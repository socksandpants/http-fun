#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

/*  Author: Ethan Jones
*   Date: 2025-12-27
*   Program: http-server
*     This is an http/1.1 server written in C17 made for a machine with Debian 13 (Trixie) installed.
*     It waits for an http request on a given port.
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
#define GET "GET"

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
// QUESTION????
// WHAT DOES THE SERVER DO WHEN A MESSAGE IS IN UNICODE?

char* process_message(char* message) {
  // check to see if the message is in ASCII
  char ascii_flag = 0;
  for (int x = 0; x < strlen(message); x++) {
    if (message[x] < 0 || message[x] > 128) {
      graceful_error();
      return -1;
    }
  }

  char[256] response;
  memset(&reponse, 0, sizeof(response) * 256);

  // request line = method-token, single space, request-target, single space, http version
  // request target functionality dependent on its format

  // status line = HTTP-version SP status-code SP [ reason-phrase ]
  // field line = field-name ":" OWS field-value OWS
  // message body = ...

  char* method_token;

  method_token = strtok(request, "");
  if (token == NULL) graceful_error(); // empty message

  // need to convert strings to ints for a switch case, can either hash OR do basic string comparisons

  // case sensitive?
  switch (method_token) {
    case GET:
      token = strtok(NULL, "");
      break;
    default:
      // unknown method-token
      break;
  }

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
  if (!(status == 0)) handle_error("getaddrinfo err, %s\n", gai_strerror(status));

  sockfd = socket(serv_info->ai_family, serv_info->ai_socktype, serv_info->ai_protocol);
  if (sockfd == -1) handle_error("socket err\n");

  status = bind(sockfd, serv_info->ai_addr, serv_info->ai_addrlen);
  if (status == -1) handle_error("bind err\n");

  while (1) {
    // set up TCP connection on new_fd
    addr_size = sizeof conn_addr;
    new_fd = setup_conn(sockfd, conn_addr, addr_size);

    memset(&message, 0, sizeof(message));
    bytes_read = recv(new_fd, &message, 256, 0);
    if (bytes_read == 0) // END OF REQUESTS, TERMINATE CONNECTION
    if (bytes_read < 0) handle_error("recv err\n");
    // below line should result in server sending correct error message
    if (bytes_read > 256) handle_error("too large of a message...");

    // process an HTTP message
    response = process_message(message);
    printf("    %s\n\n", response);

    // send HTTP response

    // end TCP connection, different for 1.1?
    status = close(new_fd);
    if (!(status == 0)) handle_error("close err\n");
  }

  status = freeaddrinfo(serv_info);
  if (!(status == 0)) handle_error("freeaddrinfo err, %s\n", gai_strerror(status));

  return 0;
}
