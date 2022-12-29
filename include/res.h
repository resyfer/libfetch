#ifndef __LIBFETCH_RES_H
#define __LIBFETCH_RES_H

#include <stdbool.h>
#include <libhmap/hmap.h>

typedef struct {
	char *code;
	hmap_t *headers;
	char *body;
} res_ok_t;

typedef struct {
	char *code;
	char *msg;
} res_err_t;

typedef struct {
	bool ok;
	res_ok_t *data;
	res_err_t *err;
} res_t;

void status_codes_init();
void parse_res(char *buf, res_t *res);

char* get_res_header(res_t *res, const char *header);

#endif