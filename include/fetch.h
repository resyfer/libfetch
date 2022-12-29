#ifndef __LIBFETCH_H
#define __LIBFETCH_H

#include <libhmap/hmap.h>
#include <libcol/col.h>

#include "url.h"
#include "global.h"
#include "res.h"

#define LOCALHOST "127.0.0.1"

typedef struct {
	url_t *url;
	const char *method;
	const char *body;
	hmap_t *headers;
} req_t;

extern hmap_t *status_codes;

void add_req_header(req_t *req, const char *header, const char* value);
char* get_req_header(req_t *req, const char *header);

res_t* fetch(req_t *req);
void free_fetch(req_t *req, res_t *res);

#endif