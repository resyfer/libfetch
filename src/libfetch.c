#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

#include <libcol.h>

#include <include/url.h>
#include <include/libfetch.h>
#include <include/global.h>
#include <include/res.h>

// Ref: https://www.rfc-editor.org/rfc/rfc9110.html#section-9.1-7
const char *allowed_methods[] = {
	"GET",
	"HEAD",
	"POST",
	"PUT",
	"DELETE",
	"CONNECT",
	"OPTIONS",
	"TRACE",
	0
};

bool
check_method(struct fetch_req *req)
{
	if(!req) {
		return false;
	}

	for(int i = 0; allowed_methods[i] != 0; i++) {
		if(req->method == allowed_methods[i]) {
			return true;
		}
	}

	return false;
}

struct fetch_res*
fetch(struct fetch_req *req)
{
	struct map_itr *itr;
	struct node *entry;

	// Preparing Status Codes
	status_codes_init(&status_codes);

	// Preparing Request
	char content_length[5] = {0};
	sprintf(content_length, "%d", strlen(req->body));

	char http_req[MAX] = {0};
	int j = 0;

	//Request-Line
	j += sprintf(http_req + j, "%s %s", req->method, (req->url->path) ? req->url->path : "/");
	if(req->url->queries->size != 0) {
		j += sprintf(http_req + j, "?");
	}
	itr = new_map_itr(req->url->queries);
	while((entry = itr_adv(itr)) != NULL) {
		j += sprintf(http_req + j, "%s=%s&", entry->key, entry->value);
	}
	if(req->url->queries->size != 0) {
		j--;
	}
	j += sprintf(http_req + j, " HTTP/1.1\r\n");


	// Headers
	j += sprintf(http_req + j, "Host: %s\r\n", req->url->host_name);
	j += sprintf(http_req + j, "Content-Length: %s\r\n", content_length);
	j += sprintf(http_req + j, "Connection: close\r\n", content_length);

	itr = new_map_itr(req->headers);
	while((entry = itr_adv(itr)) != NULL) {
		j += sprintf(http_req + j, "%s: %s\r\n", entry->key, entry->value);
	}

	// Body
	j += sprintf(http_req + j, "\r\n", req->url->host_name);
	j += sprintf(http_req + j, "%s", req->body);

	// Request
	const int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1) {
		PCOL("Can't establish socket connection with server", RED);
		exit(1);
	}

	struct sockaddr_in serv_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(req->url->port),
		.sin_addr.s_addr = req->url->ip,
	};

	if(connect(sock_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
		PCOL("Connection Error\n", RED);
		exit(1);
	}

	write(sock_fd, http_req, strlen(http_req));

	// Response
	char buf[MAX] = {0};
	read(sock_fd, buf, sizeof(buf));


	struct res_ok *data = malloc(sizeof(struct res_ok));
	struct res_err *err = malloc(sizeof(struct res_err));

	struct fetch_res *res = malloc(sizeof(struct fetch_res));
	res->data = data;
	res->err = err;

	parse_res(buf, res);

	close(sock_fd);
	return res;
}

void
add_req_header(struct fetch_req *req, const char *header, const char* value)
{
	map_push(req->headers, header, value);
}

char*
get_req_header(struct fetch_req *req, const char *header)
{
	return map_get(req->headers, header);
}

void
free_fetch(struct fetch_req *req, struct fetch_res *res)
{
	// Free Req
	free_url(req->url);
	free_map(req->headers);

	// Free Res
	if(!res) {
		return;
	}

	if(res->ok) {
		free(res->data->body);
		free(res->data->code);
		free_map(res->data->headers);

		free(res->data);
	} else {
		free(res->err->code);
		free(res->err->msg);

		free(res->err);
	}
	free(res);
}