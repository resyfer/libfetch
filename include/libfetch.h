#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

#include <libhmap/libhmap.h>
#include <libcol/libcol.h>

#include "url.h"
#include "global.h"
#include "res.h"

#ifndef __LIBFETCH_H
#define __LIBFETCH_H

#define LOCALHOST "127.0.0.1"

struct fetch_req {
	struct url *url;
	const char *method;
	const char *body;
	struct map *headers;
};

extern struct map *status_codes;

void add_req_header(struct fetch_req *req, const char *header, const char* value);
char* get_req_header(struct fetch_req *req, const char *header);

struct fetch_res* fetch(struct fetch_req *req);
void free_fetch(struct fetch_req *req, struct fetch_res *res);

#endif