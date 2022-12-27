#include <arpa/inet.h>

#include <libhmap.h>

#ifndef __LIBFETCH_URL_H
#define __LIBFETCH_URL_H

#define HTTP_PORT 80
#define HTTPS_PORT 443

enum protocol {
  HTTP,
  HTTPS
};

struct url {
  enum protocol protocol;
  in_addr_t ip;
  char *host_name;
  u_int16_t port;
  char *path;
  struct map *queries;
};

struct url* parse_url(const char *url);

void free_url(struct url *url);

#endif