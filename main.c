#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>

/*  Author: Ethan Jones
*   Date: 2025-12-27
*   Program: http-server
*     This is an http/1.1 server written in C17 made for a machine with Debian 13 (Trixie) 
*     installed. It waits for an http request on a given port, processes the request, and 
*     creates an appropiate http response.
*
*   Resources used:
*     - RFCs 3986, 9110, 9112
*     - ISO/IEC9899:2017
*     - Beej's guide to network programming (for networking brushing up)
*     - HTTP The Definitive Guide, by Gourley and Totty published by O'Reily (2002)
*/

#define MAX_CONNECTIONS 20
#define handle_error(msg) \
  do { perror(msg); exit(EXIT_FAILURE); } while(0)

typedef struct header {
  char* name;
  char* value;
  struct header *next;
}

typedef struct {
  char* method;
  char* target_uri;
  char* version;
  Header* headers;	
  char* body;
} http_message;

// this shit will get a request
void ship_response(http_message request) {
  // METHODS
  //  get
  //  head
  //  post
  //  put
  //  options

  // need to create status codes
  return;
}

// listen on a port and accept a connection to it (BLOCKING)
int setup_conn(int sockfd, struct sockaddr_storage conn_addr, socklen_t addr_size) {
  int new_fd, status;
  status = listen(sockfd, MAX_CONNECTIONS);
  if (!(status == 0)) handle_error("listen err\n");

  printf("    Now waiting for a request on port...\n");

  new_fd = accept(sockfd, (struct sockaddr*) &conn_addr, &addr_size);
  if (new_fd < 0) handle_error("accept err\n");
  return new_fd;
}

// Function to check if a string is ascii
bool is_ascii(const char* string) {
  for (; *string; string++) {
    if ((unsigned char) *string > 127) return false;
  }
  return true;
}

/* Function is probably easy to break, try 
    - Header no value. Value no header.
    - No Header.
    - A bunch of new lines.
    - See what \r will do (if I can add it to a file somehow)
    - throwing against a wall to see if it breaks
*/
Header* headers_parse(const char* raw_headers, bool *body_bool) {
  // NOTE: body is visible in this function
  Header* head = NULL;
  char* position = raw_headers; // NULL raw_headers case?
  int length = strlen(raw_headers);
  int colon_index;
  int nl_index;

  while () {
    /*
      Find colon index
      Copy memory from position to colon index -> head.name
      Update position to colon index + 1
      Find new line index
      Copy memory from position to new line index -> head.value
      Update position to new line index + 1
      Next char a new line? DONE
    */
  }
  // need HOST header?
  
  return head;
}

// This function receives read bytes from the open port and returns the parsed
// http_message.
http_message request_parse(const char* raw_request, int request_length) {
  // request line = method-token, single space, request-target, single space, http version
  //    request target functionality dependent on its format
  //    body is usually absent
  http_message request;
  char* token;

  if (!(is_ascii(raw_request))) return "err: non-ascii format";

  token = strtok(raw_request, " ");
  if (token == NULL) return "empty method head"; 
  request.method = token;
	
  token = strtok(NULL, " ");
  if (token == NULL) return "empty target uri";
  request.target_uri = token;

  token = strtok(NULL, " ");
  if (token == NULL) return "empty version"; 
  request.version = token;

  char* position = memchr(raw_request, '\n', request_length); // find first newline
  if (position == NULL) return "no second line";

  bool body_bool = false;
  request.headers = headers_parse(position + 1, &body_bool);
  // ^^^ this will be the first header of a linked list
  
  // if (body_bool) { body_read(); }

  return request;
}

int main (int argc, char** argv) {
  struct addrinfo hints;
  struct addrinfo* serv_info;
  struct sockaddr_storage conn_addr;
  socklen_t addr_size;
  int sockfd, new_fd, bytes_read;
  char message[256];
  http_message request;
  char* port;
  int status = -1;

  if (argc != 2) {
    perror("You must feed this server a port (don't use 80 unless you configure stuff right)!\n   $ ./http-server ####\n");
    exit(EXIT_FAILURE);
  }
  port = argv[1];

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  status = getaddrinfo(NULL, port, &hints, &serv_info);
  if (!(status == 0)) { 
    char* err_msg;
    sprintf(err_msg, "getaddrinfo err, %s\n", gai_strerror(status));
    handle_error(err_msg);
  }

  sockfd = socket(serv_info->ai_family, serv_info->ai_socktype, serv_info->ai_protocol);
  if (sockfd == -1) handle_error("socket err\n");

  status = bind(sockfd, serv_info->ai_addr, serv_info->ai_addrlen);
  if (status == -1) handle_error("bind err\n");

  /* infinite loop, change to be able to handle multiple requests to one connection */

  // TCP_NODELAY to disable Nagle's algorithm for quicker TCP messaging
  // epoll()

  while (1) {
    // set up TCP connection on new_fd
    addr_size = sizeof conn_addr;
    new_fd = setup_conn(sockfd, conn_addr, addr_size);
    memset(&message, 0, sizeof(message));

    bytes_read = recv(new_fd, &message, 256, 0);

    if (bytes_read == 0) { /* how to handle this? */  } /* socket connection terminated */
    if (bytes_read < 0) handle_error("recv err\n");
    if (bytes_read > 256) handle_error("too large of a message...");

    request = request_parse(message, bytes_read);
    ship_response(request);

    // end TCP connection
    status = close(new_fd);
    if (!(status == 0)) handle_error("close err\n");
  }

  status = freeaddrinfo(serv_info);
  if (!(status == 0)) handle_error("freeaddrinfo err, %s\n", gai_strerror(status));

  return 0;
}
