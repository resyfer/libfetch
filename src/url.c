#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>

#include <include/url.h>

// TODO: Better URL parser
struct url*
parse_url(const char *url)
{
	int n = strlen(url);
	struct url *parsed_url = malloc(sizeof(struct url));

	char proto[] = "https";
	int m = strlen(proto);

	// Check protocol
	int index = 0;
	for(; index<m; index++) {
		if(index < m-1 && url[index] != proto[index]) {
			parsed_url->protocol = HTTP; //Default
			parsed_url->port = HTTP_PORT;
			index = 0;
			break;
		} else if(index == m-1 && url[index] == proto[index] && url[index+1] == ':' && url[index+2] == '/' && url[index+3] == '/') {
			parsed_url->protocol = HTTPS;
			parsed_url->port = HTTPS_PORT;
			index = index + 3;
		} else if (index == m-1 && url[index] == ':' && url[index+1] == '/' && url[index+2] == '/') {
			parsed_url->protocol = HTTP;
			parsed_url->port = HTTP_PORT;
			index = index + 2;
		} else if (index == m-1) {
			index = 0;
			parsed_url->protocol = HTTP;
			parsed_url->port = HTTP_PORT;
			break;
		}
	}

	// Get URL
	char *host = calloc(n+1, sizeof(char));
	int start = index;
	for(; index < n && !(
		url[index] == '/' ||
		url[index] == '#' ||
		url[index] == '?' ||
		url[index] == ':'
		); index++) {
		host[index - start] = url[index];
	}
	host = realloc(host, sizeof(char) * (strlen(host) + 1));

	// DNS Resolution
	struct hostent *h_ent = gethostbyname(host);
	if(!h_ent) {
		return NULL;
	}
	parsed_url->ip = ((struct in_addr*) h_ent->h_addr_list[0])->s_addr;
	parsed_url->host_name = host;

	// Get Port
	char port[5] = {0}; // Max port is 65535
	if(index < n && url[index] == ':') {
		index++;
		start = index;
		for(; index < n && !(url[index] == '/' || url[index] == '#' || url[index] == '?'); index++) {
			port[index - start] = url[index];
		}
		parsed_url->port = atoi(port);
	}
	// If port not mentioned, default protocol ports are already assigned

	// Get Url
	char *path = calloc(n+1, sizeof(char));
	if(index < n && url[index] == '/') {
		start = index;
		for(; index < n && !(url[index] == '?' || url[index] == '#'); index++) {
			path[index - start] = url[index];
		}
	}
	path = realloc(path, sizeof(char) * strlen(path));
	parsed_url->path = path;

	// Get Queries
	index++;
	struct map *queries = new_map();
	char *key = calloc(n+1, sizeof(char)), *value = calloc(n+1, sizeof(char));
	for(; index < n; index++) {
		// Key
		start = index;
		for(; index < n && !(url[index] == '=' || url[index] == '#'); index++) {
			key[index-start] = url[index];
		}

		// Value
		index++;
		start = index;
		for(; index < n && !(url[index] == '&' || url[index] == '#'); index++) {
			value[index-start] = url[index];
		}

		map_push(queries, key, value);

		memset(key, 0, (n+1) * sizeof(char));
		memset(value, 0, (n+1) * sizeof(char));
	}
	free(key);
	free(value);

	parsed_url->queries = queries;

	return parsed_url;
}

void
free_url(struct url *url)
{
	free(url->host_name);
	free(url->path);
	free_map(url->queries);

	free(url);
}