#include <libfetch.h>
#include <stdio.h>

int main() {

  struct url *url = parse_url("http://localhost:5000/path?hello=world");

  struct fetch_req req = {
    .url = url,
    .body = "Hello World",
    .method = "GET",
    .headers = new_map()
  };
  add_req_header(&req, "Content-Type", "text/plain");

  struct fetch_res *res = fetch(&req);

  if(res->ok) {
    printf("Code -> %s\n", res->data->code);
    printf("Body -> %s\n", res->data->body);

    print_map(res->data->headers); // Documentation can be found at `libhmap`
  } else {
    printf("Code -> %s\n", res->err->code);
    printf("Message -> %s\n", res->err->msg);
  }

  free_fetch(&req, res);

  return 0;
}