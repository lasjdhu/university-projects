/*
 * @file dns.c
 * @brief Work with DNS server, signals, threads and sockets.
 * Constructing replies based on domain names and input conditions
 * @author Dmitrii Ivanushkin (xivanu00)
 */
#include "dns.h"

int SOCKFD = -1;

/*
 * handle_signal
 * @brief Closes socket when sa in start_server emits a exiting signal
 * @returns void
 */
void handle_signal() {
  if (SOCKFD != -1) {
    close(SOCKFD);
  }
  printf("Exiting\n");
  exit(EXIT_SUCCESS);
}

/*
 * reject_query
 * @brief Rejects query setting 4 as rcode
 * @returns void
 */
void reject_query(int sockfd, unsigned char *request, size_t size,
                  struct sockaddr_in *cliaddr, socklen_t len) {
  unsigned char reply[MAX_STRING_LENGTH];
  // NXDOMAIN (4)
  size_t reply_len = build_dns_reply(request, size, reply, 4);
  sendto(sockfd, reply, reply_len, 0, (struct sockaddr *)cliaddr, len);
}

/*
 * block_query
 * @brief Blocks query setting 3 as rcode
 * @returns void
 */
void block_query(int sockfd, unsigned char *request, size_t size,
                 struct sockaddr_in *cliaddr, socklen_t len) {
  unsigned char reply[MAX_STRING_LENGTH];
  // REFUSED (3)
  size_t reply_len = build_dns_reply(request, size, reply, 3);
  sendto(sockfd, reply, reply_len, 0, (struct sockaddr *)cliaddr, len);
}

/*
 * forward_query
 * @brief Sets up a new socket and forwards a response caught by the program
 * back
 * @returns void
 */
void forward_query(int sockfd, unsigned char *request, size_t size,
                   struct sockaddr_in *cliaddr, socklen_t len,
                   struct sockaddr_in *resolver, socklen_t rlen) {

  unsigned char resp[MAX_STRING_LENGTH];

  // Forwarding socket
  int forward_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (forward_sock < 0) {
    fprintf(stderr, "Failed to create forwarding socket\n");
    exit(EXIT_FAILURE);
  }

  // Request
  sendto(forward_sock, request, size, 0, (struct sockaddr *)resolver, rlen);

  // Response
  struct sockaddr_in resp_addr;
  socklen_t resp_len = sizeof(resp_addr);
  ssize_t n = recvfrom(forward_sock, resp, sizeof(resp), 0,
                       (struct sockaddr *)&resp_addr, &resp_len);

  // Send response
  if (n > 0) {
    sendto(sockfd, resp, n, 0, (struct sockaddr *)cliaddr, len);
  }

  close(forward_sock);
}

/*
 * handle_request
 * @brief Gets domain from request, reads QTYPE and reacts based on QTYPE and
 * domain name in headers
 * @returns *void
 */
void *handle_request(void *arg) {
  struct thread_args *t = arg;

  char *domain = get_domain_from_dns_request(t->buffer, t->n);
  if (!domain) {
    free(t);
    return NULL;
  }

  uint16_t qtype = get_qtype(t->buffer, t->n);
  if (t->config->verbose) {
    if (qtype == 1) {
      printf("Query: %s, type: A, result: ", domain);
    } else {
      printf("Query: %s, type: Other, result: ", domain);
    }
  }

  if (qtype != 1) {
    reject_query(t->sockfd, t->buffer, t->n, &t->cliaddr, t->len);
    if (t->config->verbose) {
      printf("Rejected\n\n");
    }
  } else if (is_blocked(domain, t->blocked_domains)) {
    block_query(t->sockfd, t->buffer, t->n, &t->cliaddr, t->len);
    if (t->config->verbose) {
      printf("Blocked\n\n");
    }
  } else {
    forward_query(t->sockfd, t->buffer, t->n, &t->cliaddr, t->len, &t->resolver,
                  sizeof(t->resolver));
    if (t->config->verbose) {
      printf("Forwarded\n\n");
    }
  }

  free(domain);
  free(t);
  return NULL;
}

/*
 * start_server
 * @brief Sets up UPD socket, server address, binds to a port, resolves IP
 * address and runs a loop where it sets up threads that handle query processing
 * @returns exit code
 */
int start_server(char **blocked_domains, struct config *config) {
  // UDP socket
  SOCKFD = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (SOCKFD < 0) {
    fprintf(stderr, "Failed to create socket\n");
    exit(EXIT_FAILURE);
  }

  // Server address
  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(config->port);

  // Port biding
  if (bind(SOCKFD, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "Failed to bind to port %d\n", config->port);
    exit(EXIT_FAILURE);
  }

  // Resolver address
  struct sockaddr_in resolver;
  memset(&resolver, 0, sizeof(resolver));
  resolver.sin_family = AF_INET;
  resolver.sin_port = htons(53);
  inet_pton(AF_INET, config->server, &resolver.sin_addr);

  // Graceful shutdown via signals
  struct sigaction sa;
  sa.sa_handler = handle_signal;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);

  // Buffer for packets
  unsigned char buffer[MAX_STRING_LENGTH];
  struct sockaddr_in cliaddr;
  socklen_t len;

  while (1) {
    len = sizeof(cliaddr);
    ssize_t n = recvfrom(SOCKFD, buffer, MAX_STRING_LENGTH, 0,
                         (struct sockaddr *)&cliaddr, &len);
    // Invalid
    if (n < 12) {
      continue;
    }

    struct thread_args *t = malloc(sizeof(*t));
    if (!t) {
      continue;
    }

    t->sockfd = SOCKFD;
    memcpy(t->buffer, buffer, n);
    t->n = n;
    t->cliaddr = cliaddr;
    t->len = len;
    t->resolver = resolver;
    t->blocked_domains = blocked_domains;
    t->config = config;

    // Create threads
    pthread_t thread;
    pthread_create(&thread, NULL, handle_request, t);
    pthread_detach(thread);
  }

  return 0;
}
