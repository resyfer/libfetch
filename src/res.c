#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <include/res.h>

hmap_t *status_codes;

void
status_codes_init()
{
	status_codes = hmap_new_cap(15);

	// 1xx - Informational
	hmap_push(status_codes, "100", (void*) "Continue");
	hmap_push(status_codes, "101", (void*) "Switching Protocols");

	// 2xx - Success
	hmap_push(status_codes, "200", (void*) "OK");
	hmap_push(status_codes, "201", (void*) "Created");
	hmap_push(status_codes, "202", (void*) "Accepted");
	hmap_push(status_codes, "203", (void*) "Non-Authoritative Information");
	hmap_push(status_codes, "204", (void*) "No Content");
	hmap_push(status_codes, "205", (void*) "Reset Content");
	hmap_push(status_codes, "206", (void*) "Partial Content");

	// 3xx - Redirection
	hmap_push(status_codes, "300", (void*) "Multiple Choices");
	hmap_push(status_codes, "301", (void*) "Moved Permanently");
	hmap_push(status_codes, "302", (void*) "Found");
	hmap_push(status_codes, "303", (void*) "See Other");
	hmap_push(status_codes, "304", (void*) "Not Modified");
	hmap_push(status_codes, "305", (void*) "Use Proxy");
	hmap_push(status_codes, "307", (void*) "Temporary Redirect");

	// 4xx - Client Error
	hmap_push(status_codes, "400", (void*) "Bad Request");
	hmap_push(status_codes, "401", (void*) "Unauthorized");
	hmap_push(status_codes, "402", (void*) "Payment Required");
	hmap_push(status_codes, "403", (void*) "Forbidden");
	hmap_push(status_codes, "404", (void*) "Not Found");
	hmap_push(status_codes, "405", (void*) "Method Not Allowed");
	hmap_push(status_codes, "406", (void*) "Not Acceptable");
	hmap_push(status_codes, "407", (void*) "Proxy Authentication Required");
	hmap_push(status_codes, "408", (void*) "Request Time-out");
	hmap_push(status_codes, "409", (void*) "Conflict");
	hmap_push(status_codes, "410", (void*) "Gone");
	hmap_push(status_codes, "411", (void*) "Length Required");
	hmap_push(status_codes, "412", (void*) "Precondition Failed");
	hmap_push(status_codes, "413", (void*) "Request Entity Too Large");
	hmap_push(status_codes, "414", (void*) "Request-URI Too Large");
	hmap_push(status_codes, "415", (void*) "Unsupported Media Type");
	hmap_push(status_codes, "416", (void*) "Requested range not satisfiable");
	hmap_push(status_codes, "417", (void*) "Expectation Failed");

	// 5xx - Server Error
	hmap_push(status_codes, "500", (void*) "Internal Server Error");
	hmap_push(status_codes, "501", (void*) "Not Implemented");
	hmap_push(status_codes, "502", (void*) "Bad Gateway");
	hmap_push(status_codes, "503", (void*) "Service Unavailable");
	hmap_push(status_codes, "504", (void*) "Gateway Time-out");
	hmap_push(status_codes, "505", (void*) "HTTP Version not supported");
}

void
parse_res(char *buf, res_t *res)
{

	if(!buf || !res) {
		res = NULL;
		return;
	}

	int n = strlen(buf);
	int index = 0, start;

	// Status Code
	for(; index < n && buf[index] != ' '; index++);
	index++;

	start = index;
	char *code = calloc(3, sizeof(char));
	for(; index < n && buf[index] != ' '; index++) {
		code[index - start] = buf[index];
	}

	char *status = hmap_get(status_codes, code);

	if(code[0] != '2') {
		res->ok = false;
		res->data = NULL;
		res->err->code = code;
		res->err->msg = status;
		return;
	}

	res->ok = true;
	res->err = NULL;
	res->data->code = code;

	// Headers
	for(;index < n && buf[index] != '\n'; index++); // Escaping Status line
	index++;

	res->data->headers = hmap_new_cap(5);

	char *key, *value;
	for(;index<n; index++) {
		key = calloc(150, sizeof(char));
		value = calloc(150, sizeof(char));

		if((buf[index] == '\r' && buf[index+1] == '\n') || (buf[index] == '\n')) {
			break;
		}

		// Key
		start = index;
		for(; index < n && buf[index] != ':'; index++) {
			key[index - start] = buf[index];
		}

		if(buf[index+1] = ' ') {
			index++;
		}
		index++;

		// Value
		start = index;
		for(; index < n && !(buf[index] == '\r' || buf[index] == '\n'); index++) {
			value[index - start] = buf[index];
		}

		if(buf[index] == '\r') {
			index++;
		}

		key = realloc(key, sizeof(char) * (strlen(key) + 1));
		value = realloc(value, sizeof(char) * (strlen(value) + 1));

		hmap_push(res->data->headers, key, (void *) value);
	}

	// Escaping lone CRLF
	if(buf[index] == '\r' && buf[index+1] == '\n') {
		index+=2;
	} else if (buf[index] == '\n') {
		index++;
	}

	// Body
	start = index;
	char *body = calloc(n, sizeof(char));
	for(; index < n; index++) {
		body[index - start] = buf[index];
	}
	body = realloc(body, sizeof(char) * (strlen(body) + 1));
	res->data->body = body;
}

char*
get_res_header(res_t *res, const char *header)
{
	if(!res->ok) {
		return NULL;
	}

	return hmap_get(res->data->headers, header);
}