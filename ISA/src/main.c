/*
 * @file main.c
 * @author Dmitrii Ivanushkin (xivanu00)
 */
#include "dns.h"
#include "helpers.h"

/*
 * main
 * @brief Entry point
 * @returns Exit status
 */
int main(int argc, char *argv[]) {
  // Read arguments
  struct config config = {0};
  parse_arguments(argc, argv, &config);

  // Read file
  char **domains = read_domains(config.filter_file);

  // Start DNS server
  int result = start_server(domains, &config);

  // Exit
  free_domains(domains);
  return result;
}
