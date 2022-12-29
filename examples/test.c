#include <libfetch/fetch.h>
#include <stdio.h>

int main() {

  url_t *url = parse_url("http://localhost:5000/path?hello=world");

  req_t req = {
    .url = url,
    .body = "Hello World",
    .method = "GET",
    .headers = hmap_new()
  };
  add_req_header(&req, "Content-Type", "text/plain");

  res_t *res = fetch(&req);

  if(res->ok) {
    printf("Code -> %s\n", res->data->code);
    printf("Body -> %s\n", res->data->body);

    printf("Length: %s\n", get_res_header(res, "Content-Length"));

  } else {
    printf("Code -> %s\n", res->err->code);
    printf("Message -> %s\n", res->err->msg);
  }

  free_fetch(&req, res);

  return 0;
}