#include <stdbool.h>

#ifndef __LIBFETCH_RES_H
#define __LIBFETCH_RES_H

struct res_ok {
	char *code;
	struct map *headers;
	char *body;
};

struct res_err {
	char *code;
	char *msg;
};

struct fetch_res {
	bool ok;
	struct res_ok *data;
	struct res_err *err;
};

void status_codes_init();
void parse_res(char *buf, struct fetch_res *res);

#endif