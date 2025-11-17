/*
 * @file helpers.h
 * @brief Header file with lib imports, function, struct and constants
 * definitions
 * @author Dmitrii Ivanushkin (xivanu00)
 */
#ifndef HELPERS_H
#define HELPERS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEFAULT_PORT 53
#define MAX_STRING_LENGTH 256

struct config {
  char server[MAX_STRING_LENGTH];
  int port;
  char filter_file[MAX_STRING_LENGTH];
  bool verbose;
};

void parse_arguments(int argc, char *argv[], struct config *config);
char **read_domains(char *filename);
void free_domains(char **domains);
void print_usage();

#endif
