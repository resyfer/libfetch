#include <include/fetch.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

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
check_method(req_t *req)
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

res_t*
fetch(req_t *req)
{
	hmap_itr_t *itr;
	hmap_node_t *entry;

	// Preparing Status Codes
	status_codes_init(&status_codes);

	// Preparing Request
	char content_length[5] = {0};
	sprintf(content_length, "%d", strlen(req->body));

	char http_req[MAX] = {0};
	int j = 0;

	//Request-Line
	j += sprintf(http_req + j, "%s %s",
				req->method,
				(req->url->path) ? req->url->path : "/");

	if(req->url->queries->size != 0) {
		j += sprintf(http_req + j, "?");
	}

	itr = hmap_itr_new(req->url->queries);
	while((entry = hmap_itr_adv(itr)) != NULL) {
		j += sprintf(http_req + j, "%s=%s&",
					entry->key, (char*) entry->value);
	}
	if(req->url->queries->size != 0) {
		j--;
	}
	j += sprintf(http_req + j, " HTTP/1.1\r\n");


	// Headers
	j += sprintf(http_req + j, "Host: %s\r\n", req->url->host_name);
	j += sprintf(http_req + j, "Content-Length: %s\r\n", content_length);
	j += sprintf(http_req + j, "Connection: close\r\n", content_length);

	itr = hmap_itr_new(req->headers);
	while((entry = hmap_itr_adv(itr)) != NULL) {
		j += sprintf(http_req + j, "%s: %s\r\n",
					entry->key, (char *) entry->value);
	}

	// Body
	j += sprintf(http_req + j, "\r\n", req->url->host_name);
	j += sprintf(http_req + j, "%s", req->body);

	// Request
	const int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_fd == -1) {
		char *text_red_bold = col_str_style(RED, BOLD);
		printf("\n%sCan't establish socket connection with server.%s\n",
				text_red_bold, RESET);
		free(text_red_bold);
		exit(1);
	}

	struct sockaddr_in serv_addr = {
		.sin_family = AF_INET,
		.sin_port = htons(req->url->port),
		.sin_addr.s_addr = req->url->ip,
	};

	if(connect(sock_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
		char *text_red_bold = col_str_style(RED, BOLD);
		printf("\n%sConnection Error.%s\n",
				text_red_bold, RESET);
		free(text_red_bold);
		exit(1);
	}

	write(sock_fd, http_req, strlen(http_req));

	// Response
	char buf[MAX] = {0};
	read(sock_fd, buf, sizeof(buf));


	res_ok_t *data = malloc(sizeof(res_ok_t));
	res_err_t *err = malloc(sizeof(res_err_t));

	res_t *res = malloc(sizeof(res_t));
	res->data = data;
	res->err = err;

	parse_res(buf, res);

	close(sock_fd);
	return res;
}

void
add_req_header(req_t *req, const char *header, const char* value)
{
	hmap_push(req->headers, header, (void*) value);
}

char*
get_req_header(req_t *req, const char *header)
{
	return (char *) hmap_get(req->headers, header);
}

void
free_fetch(req_t *req, res_t *res)
{
	// Free Req
	free_url(req->url);
	hmap_free(req->headers);

	// Free Res
	if(!res) {
		return;
	}

	if(res->ok) {
		free(res->data->body);
		free(res->data->code);

		hmap_itr_t *itr = hmap_itr_new(res->data->headers);
		hmap_node_t *elem;
		while((elem = hmap_itr_adv(itr)) != NULL) {
			free(elem->value);
		}

		hmap_free(res->data->headers);


		free(res->data);
	} else {
		free(res->err->code);
		free(res->err->msg);

		free(res->err);
	}
	free(res);
}