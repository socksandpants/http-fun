#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

/*  Author: Ethan Jones
*   Date: 2025-12-27
*   Program: http-server
*     This is an http/1.1 server written in C17 made for a machine with Debian 13 (Trixie) installed.
*     It waits for an http request on a given port, processes the request, and creates an appropiate 
*			http response.
*
*   Resources used:
*     - RFCs 3986, 9110, 9112
*     - ISO/IEC9899:2017
*     - Beej's guide to network programming (for networking brushing up)
*     - HTTP The Definitive Guide, by Gourley and Totty published by O'Reily (2002)
*/

#define handle_error(msg) \
  do { perror(msg); exit(EXIT_FAILURE); } while(0)
#define NUM_CONNECTIONS 20

// simple linked list for headers
typedef struct Header {
  char* name;
  char* value;
  struct Header *next;
}

// generalize to http_message?
typedef struct {
  char* method;
  char* target_uri;
  char* version;
  Header* headers;	
  char* body;
} http_request;

typedef struct {
  char* method;
  char* target_uri;
  char* version;
  Header* headers;	
  char* body;
} http_response;

// Function to listen on a port and accept a connection to it (BLOCKING)
int setup_conn(int sockfd, struct sockaddr_storage conn_addr, socklen_t addr_size) {
  int new_fd, status;
  status = listen(sockfd, NUM_CONNECTIONS);
  if (!(status == 0)) handle_error("listen err\n");

  printf("    Now waiting for a request on port...\n");

  new_fd = accept(sockfd, (struct sockaddr*) &conn_addr, &addr_size);
  if (new_fd < 0) handle_error("accept err\n");
  return new_fd;
}

// This function is called when the server receives an invalid message and
// wants to handle it. I think it always sends and error code but there's
// probably an edge case for throwing out a request entirely.
void graceful_error() {
  return;
}

// Function to check if a string is ascii
bool is_ascii(char* string) {
  for (; *string; string++) {
    if ((unsigned char) *string > 127) return false;
  }
  return true;
}

// TODO: need to create a void free_header(Header* header)

Header* headers_parse(const char* raw) {
  Header* head = NULL;
  Header* tail = NULL;
  char* token;
  // TODO: DON'T USE strtok() HERE, USE memchr(), also maybe check for \r\n
  // bs
  
  return ;
}

// This function receives read bytes from the open port and returns the
// full http response.
http_response request_parse(char* raw) {
  // request line = method-token, single space, request-target, single space, http version
  //
  // request target functionality dependent on its format
  // required HOST header
  // body is usually absent

  // status line = HTTP-version SP status-code SP [ reason-phrase ]
  // field line = field-name ":" OWS field-value OWS
  // message body = ...
  // Content-Length or Transfer-Encoding expected with body
  http_request request;
  http_response response;
  char* token;
	
  if (!(is_ascii(raw))) return "err: non-ascii format";

  token = strtok(raw, " ");
  if (token == NULL) return "empty method head"; 
  request.method = token;
	
  token = strtok(NULL, " ");
  if (token == NULL) return "empty target uri";
  request.target_uri = token;

  token = strtok(NULL, " ");
  if (token == NULL) return "empty version"; 
  request.version = token;
	
  token = strchr(raw, "\n");
  if (token == NULL) return "no second line";
  token++;
  // headers_parse()?
  // body_read()?

  // REQUEST PARSED, CREATE RESPONSE

  // PLAN
  //  1. Make server create empty messages with nothing in the message body.
  //      - request line
  //      - request target creation
  //      - status line
  //      - field line

  return response;
}

int main (int argc, char** argv) {
  struct addrinfo hints;
  struct addrinfo* serv_info;
  struct sockaddr_storage conn_addr;
  socklen_t addr_size;
  int sockfd, new_fd, bytes_read;
  char message[256];
  char* response;
  char* port;
  int status = -1;

	// Check for Ruby? I can exec() call the ruby interpreter for a version
	
  if (argc != 2) {
    perror("You must feed this server a port (don't use 80 unless you configure stuff right)!\n   $ ./http-server ####\n");
    exit(EXIT_FAILURE);
  }
  port = argv[1];

	// server_init()?
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
  /* TODO LATER: implement p_threads so that they can handle multiple connections */

  // server_run()?
  while (1) {
    // set up TCP connection on new_fd
    addr_size = sizeof conn_addr;
    new_fd = setup_conn(sockfd, conn_addr, addr_size);

    memset(&message, 0, sizeof(message));
    bytes_read = recv(new_fd, &message, 256, 0);
    if (bytes_read == 0) { /* how to handle this? */  } /* socket connection terminated */
    if (bytes_read < 0) handle_error("recv err\n");
    if (bytes_read > 256) handle_error("too large of a message...");

    // process an HTTP message
    response = request_parse(message);
    printf("    %s\n\n", response);
		
    // send HTTP response

    // end TCP connection, DIFFERENT for 1.1
    status = close(new_fd);
    if (!(status == 0)) handle_error("close err\n");
  }

  status = freeaddrinfo(serv_info);
  if (!(status == 0)) handle_error("freeaddrinfo err, %s\n", gai_strerror(status));

  return 0;
}
