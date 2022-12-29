# libfetch

A C library similar to the [fetch API of browsers](https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API). Pretty easy to use. For instructions on usage, please go [here](#usage).

For installation, please go [here](#installation). For documentation, please go [here](#documentation).

**NOTE**: Only supports HTTP connections as of now.

## Usage

### TL;DR

The shown below is a tl;dr version. Proper instructions are [here](#step-1)

```c
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
```

Compile and run:

```bash
$ gcc test.c -o test.o -lfetch -lcol -lhmap
$ ./test.o
```

### Step 1

Import the library:

```c
#include <libfetch/fetch.h>
```

**NOTE**: Link the library and dependencies when compiling

```bash
$ gcc a.c -o a.o -lfetch -lcol -lhmap
```

### Step 2

Provide the URL and parse it:

```c
url_t *url = parse_url("http://localhost:5000/path?hello=world");
```

**NOTE**: If protocol is not mentioned in URL, then default is HTTP. Same goes for port, which defaults to HTTP's default port 80. It's fine if URL is an IP Address, does not need to be a DNS.

### Step 3

Define a request structure and provide the various things you need:

```c
req_t req;
req.url = url;
req.body = "Hello World";
req.method = "GET";
req.headers = hmap_new();
```

**NOTE**: `req.headers` is a hash map, and it's usage can be found at the documentation of [libhmap](https://github.com/resyfer/libhmap.git) if needed.

### Optional Step

Provide additional headers, like `Content-Type`:

```c
add_req_header(&req, "Content-Type", "text/plain");
```

### Step 4

Get the response after `fetch`:

```c
struct fetch_res *res = fetch(&req);
```

### Step 5

Get the various data depending on the value of `res->ok`:

```c
if(res->ok) {
  printf("Code -> %s\n", res->data->code);
  printf("Body -> %s\n", res->data->body);

  printf("Length: %s\n", get_res_header(res, "Content-Length"));

} else {
  printf("Code -> %s\n", res->err->code);
  printf("Message -> %s\n", res->err->msg);
}
```

**NOTE**: You can `Content-Type` header from `res->data->headers` using `map_get` as it is a hash map from `libhmap`, and parse the `res->data->body` accordingly. `libfetch` always provides the body in string format, but if you wish you can further parse it using a JSON parser of your choice.

### Step 6

After use, free up the resources allocated to `req` and `res`:

```c
free_fetch(&req, res);
```

## Installation

Make sure to have [make](https://www.gnu.org/software/make/), [gcc](https://www.gnu.org/software/gcc/) and [git](https://git-scm.com/) installed.

Install all the the dependencies before moving on the next step (preferably in the same order as given below):

- [libhmap](https://github.com/resyfer/libhmap.git)
- [libcol](https://github.com/resyfer/libcol.git)

**NOTE**: Their installation instructions will be provided in their documentation as well.

After installing dependencies, installation of library:

```bash
git clone https://github.com/resyfer/libfetch.git
cd libfetch
make install
cd ..
rm -rf libfetch
```

Provide the password when prompted.

## Documentation

### Parse URL
```c
url_t* parse_url(const char *url);
```

### Add or Get Request Headers
```c
void add_req_header(req_t *req, const char *header, const char* value);
char* get_req_header(req_t *req, const char *header);
```

### Fetch Request
```c
res_t* fetch(req_t *req);
```

### Get Response Headers
```c
char* get_res_header(res_t *res, const char *header);
```