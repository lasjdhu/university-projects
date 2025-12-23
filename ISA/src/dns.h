/*
 * @file dns.h
 * @brief Header file with lib imports and function definitions
 * @author Dmitrii Ivanushkin (xivanu00)
 */
#ifndef DNS_H
#define DNS_H

#include "dns_utils.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

void handle_signal();
void reject_query(int sockfd, unsigned char *request, size_t size,
                  struct sockaddr_in *cliaddr, socklen_t len);
void block_query(int sockfd, unsigned char *request, size_t size,
                 struct sockaddr_in *cliaddr, socklen_t len);
void forward_query(int sockfd, unsigned char *request, size_t size,
                   struct sockaddr_in *cliaddr, socklen_t len,
                   struct sockaddr_in *resolver, socklen_t rlen);
void *handle_request(void *arg);
int start_server(char **blocked_domains, struct config *config);

#endif
