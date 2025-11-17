/*
 * @file dns_utils.h
 * @brief Header file with lib imports, function and struct definitions
 * @author Dmitrii Ivanushkin (xivanu00)
 */
#ifndef DNS_UTILS_H
#define DNS_UTILS_H

#include "arpa/inet.h"
#include "helpers.h"

struct thread_args {
  int sockfd;
  unsigned char buffer[MAX_STRING_LENGTH];
  size_t n;
  struct sockaddr_in cliaddr;
  socklen_t len;
  struct sockaddr_in resolver;
  char **blocked_domains;
  struct config *config;
};

char *get_domain_from_dns_request(unsigned char *request, size_t size);
uint16_t get_qtype(unsigned char *request, size_t size);
size_t build_dns_reply(unsigned char *request, size_t size,
                       unsigned char *reply, uint8_t rcode);
bool is_blocked(char *domain, char **blocked_domains);

#endif
