/*
 * @file dns_utils.c
 * @brief Defines helper function for work with DNS replies and requests
 * @author Dmitrii Ivanushkin (xivanu00)
 */
#include "dns_utils.h"

/*
 * get_domain_from_dns_request
 * @brief Parses DNS request to extract a name from a header
 * @returns domain name
 */
char *get_domain_from_dns_request(unsigned char *request, size_t size) {
  char domain[MAX_STRING_LENGTH];
  size_t requestPos = 12;
  size_t domainPos = 0;

  while (requestPos < size && request[requestPos] != 0) {
    int labelLength = request[requestPos];
    requestPos++;

    if (domainPos > 0) {
      domain[domainPos] = '.';
      domainPos++;
    }

    for (int i = 0; i < labelLength && requestPos < size; i++) {
      domain[domainPos] = request[requestPos];
      domainPos++;
      requestPos++;
    }
  }

  domain[domainPos] = '\0';

  return strdup(domain);
}

/*
 * get_qtype
 * @brief Parses DNS request to extract a qtype (we need to react only to A
 * types)
 * @returns QTYPE
 */
uint16_t get_qtype(unsigned char *request, size_t size) {
  size_t pos = 12;

  while (pos < size && request[pos] != 0) {
    pos += request[pos] + 1;
  }

  if (pos < size) {
    pos++;
  }

  if (pos + 2 > size) {
    return 0;
  }

  uint16_t qtype;
  memcpy(&qtype, request + pos, sizeof(uint16_t));
  return ntohs(qtype);
}

/*
 * build_dns_reply
 * @brief Helper function to fill DNS reply with correct headers and rcode from
 * arguments
 * @returns length of reply
 */
size_t build_dns_reply(unsigned char *request, size_t size,
                       unsigned char *reply, uint8_t rcode) {
  if (size < 12) {
    return 0;
  }

  memcpy(reply, request, 12);

  // set the QR bit
  unsigned char responseFlag = reply[2];
  responseFlag = responseFlag | 0x80;
  reply[2] = responseFlag;

  // Getting rid of warning
  unsigned char rdFlag = reply[2];
  rdFlag = rdFlag & (~0x01);
  reply[2] = rdFlag;

  unsigned char originalFlags = reply[3];
  unsigned char upperBitsOnly = originalFlags & 0xF0;
  unsigned char rcodeLowerBits = rcode & 0x0F;
  reply[3] = upperBitsOnly | rcodeLowerBits;

  // Reset remaining to zero
  int i = 6;
  while (i < 12) {
    reply[i] = 0;
    i++;
  }

  size_t pos = 12;
  while (pos < size && request[pos] != 0) {
    pos += request[pos] + 1;
  }

  if (pos < size) {
    pos++;
  }

  // Skipping QTYPE and QCLASS
  if (pos + 4 <= size) {
    pos += 4;
  }

  memcpy(reply + 12, request + 12, pos - 12);

  return pos;
}

/*
 * build_dns_reply
 * @brief Helper function to compare domain from DNS header with a list of
 * blocked domains
 * @returns whether domain in blocklist or not
 */
bool is_blocked(char *domain, char **blocked_domains) {
  for (int i = 0; blocked_domains[i] != NULL; i++) {
    char *blocked = blocked_domains[i];

    size_t blockLen = strlen(blocked);
    size_t domainLen = strlen(domain);

    if (domainLen >= blockLen) {
      if (strcasecmp(domain + (domainLen - blockLen), blocked) == 0) {
        if (domainLen == blockLen || domain[domainLen - blockLen - 1] == '.') {
          return true;
        }
      }
    }
  }

  return false;
}
