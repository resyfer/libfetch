#ifndef __LIBFETCH_URL_H
#define __LIBFETCH_URL_H

#include <arpa/inet.h>
#include <libhmap/hmap.h>

#define HTTP_PORT 80
#define HTTPS_PORT 443

enum protocol {
  HTTP,
  HTTPS
};

typedef struct {
  enum protocol protocol;
  in_addr_t ip;
  char *host_name;
  u_int16_t port;
  char *path;
  hmap_t *queries;
} url_t;

url_t* parse_url(const char *url);

void free_url(url_t *url);

#endif