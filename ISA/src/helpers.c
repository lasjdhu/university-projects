/*
 * @file helpers.c
 * @brief Defines helper function for work with CLI and file
 * @author Dmitrii Ivanushkin (xivanu00)
 */
#include "helpers.h"

/*
 * print_usage
 * @brief Prints usage
 * @returns void
 */
void print_usage() {
  fprintf(stderr,
          "Usage: dns -s server [-p port] -f filter_file [-v]\n"
          "\n"
          "Options:\n"
          "\t-s server      IP address of the DNS resolver\n"
          "\t-p port        Port to listen on for incoming DNS queries\n"
          "\t-f filter_file Path to file containing blocked domains\n"
          "\t-v             Verbose mode with logging\n");
}

/*
 * parse_arguments
 * @brief Parses arguments
 * @returns void
 */
void parse_arguments(int argc, char *argv[], struct config *config) {
  config->port = DEFAULT_PORT;
  config->verbose = false;

  int opt;
  while ((opt = getopt(argc, argv, "s:p:f:v")) != -1) {
    switch (opt) {
    case 's':
      strncpy(config->server, optarg, MAX_STRING_LENGTH - 1);
      break;
    case 'p':
      config->port = atoi(optarg);
      break;
    case 'f':
      strncpy(config->filter_file, optarg, MAX_STRING_LENGTH - 1);
      break;
    case 'v':
      config->verbose = true;
      break;
    default:
      print_usage();
      exit(EXIT_FAILURE);
    }
  }

  if (config->server[0] == '\0' || config->filter_file[0] == '\0') {
    print_usage();
    exit(EXIT_FAILURE);
  }
}

/*
 * read_domains
 * @brief Reads file with blocked domain names
 * @returns pointer to array of blocked domain names
 */
char **read_domains(char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Failed to open file '%s'\n", filename);
    exit(EXIT_FAILURE);
  }

  size_t capacity = 16;
  int numdomains = 0;
  char **domains = malloc(capacity * sizeof(char *));
  if (!domains) {
    fprintf(stderr, "Failed to allocate domains array\n");
    exit(EXIT_FAILURE);
  }

  char *line = NULL;
  size_t maxbytes = 0;
  ssize_t numchars;

  while ((numchars = getline(&line, &maxbytes, file)) != -1) {
    line[strcspn(line, "\r\n")] = 0;

    // Skipping empty lines and lines with start from hashtag
    if (line[0] == '\0' || line[0] == '#')
      continue;

    domains[numdomains] = strdup(line);
    if (!domains[numdomains]) {
      fprintf(stderr, "Failed to allocate memory for line\n");
      exit(EXIT_FAILURE);
    }

    numdomains++;
    if ((size_t)numdomains >= capacity) {
      capacity *= 2;
      char **tmp = realloc(domains, capacity * sizeof(char *));
      if (!tmp) {
        fprintf(stderr, "Failed to reallocate domains array\n");
        exit(EXIT_FAILURE);
      }
      domains = tmp;
    }
  }

  free(line);
  fclose(file);

  domains[numdomains] = NULL;
  char **tmp = realloc(domains, (numdomains + 1) * sizeof(char *));
  if (tmp)
    domains = tmp;

  return domains;
}

/*
 * free_domains
 * @brief Frees the array of domain names
 * @returns void
 */
void free_domains(char **domains) {
  for (char **ptr = domains; *ptr != NULL; ptr++) {
    free(*ptr);
  }
  free(domains);
}
