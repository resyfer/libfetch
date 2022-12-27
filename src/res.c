#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <include/res.h>
#include <libhmap.h>

struct map *status_codes;

void
status_codes_init()
{
	status_codes = new_map_cap(15);

	// 1xx - Informational
	map_push(status_codes, "100", "Continue");
	map_push(status_codes, "101", "Switching Protocols");

	// 2xx - Success
	map_push(status_codes, "200", "OK");
	map_push(status_codes, "201", "Created");
	map_push(status_codes, "202", "Accepted");
	map_push(status_codes, "203", "Non-Authoritative Information");
	map_push(status_codes, "204", "No Content");
	map_push(status_codes, "205", "Reset Content");
	map_push(status_codes, "206", "Partial Content");

	// 3xx - Redirection
	map_push(status_codes, "300", "Multiple Choices");
	map_push(status_codes, "301", "Moved Permanently");
	map_push(status_codes, "302", "Found");
	map_push(status_codes, "303", "See Other");
	map_push(status_codes, "304", "Not Modified");
	map_push(status_codes, "305", "Use Proxy");
	map_push(status_codes, "307", "Temporary Redirect");

	// 4xx - Client Error
	map_push(status_codes, "400", "Bad Request");
	map_push(status_codes, "401", "Unauthorized");
	map_push(status_codes, "402", "Payment Required");
	map_push(status_codes, "403", "Forbidden");
	map_push(status_codes, "404", "Not Found");
	map_push(status_codes, "405", "Method Not Allowed");
	map_push(status_codes, "406", "Not Acceptable");
	map_push(status_codes, "407", "Proxy Authentication Required");
	map_push(status_codes, "408", "Request Time-out");
	map_push(status_codes, "409", "Conflict");
	map_push(status_codes, "410", "Gone");
	map_push(status_codes, "411", "Length Required");
	map_push(status_codes, "412", "Precondition Failed");
	map_push(status_codes, "413", "Request Entity Too Large");
	map_push(status_codes, "414", "Request-URI Too Large");
	map_push(status_codes, "415", "Unsupported Media Type");
	map_push(status_codes, "416", "Requested range not satisfiable");
	map_push(status_codes, "417", "Expectation Failed");

	// 5xx - Server Error
	map_push(status_codes, "500", "Internal Server Error");
	map_push(status_codes, "501", "Not Implemented");
	map_push(status_codes, "502", "Bad Gateway");
	map_push(status_codes, "503", "Service Unavailable");
	map_push(status_codes, "504", "Gateway Time-out");
	map_push(status_codes, "505", "HTTP Version not supported");
}

void
parse_res(char *buf, struct fetch_res *res)
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
	char *code = malloc(3 * sizeof(char));
	for(; index < n && buf[index] != ' '; index++) {
		code[index - start] = buf[index];
	}

	char *status = map_get(status_codes, code);

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

	res->data->headers = new_map_cap(5);
	char *key = calloc(100,  sizeof(char)), *value = calloc(100, sizeof(char));

	for(;index<n; index++) {
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
		map_push(res->data->headers, key, value);

		memset(key, 0, 100 * sizeof(char));
		memset(value, 0, 100 * sizeof(char));

	}
	free(key);
	free(value);

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