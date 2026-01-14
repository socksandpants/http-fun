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
*     It waits for an http request on port 80.
*
*   Resources used:
*     - RFCs 3986 and 9110
*     - ISO/IEC9899:2017
*     - Beej's guide to network programming (for networking brushing up)
*/

#define handle_error(msg) \
  do { perror(msg); exit(EXIT_FAILURE); } while(0)
#define NUM_CONNECTIONS 20

int setup_conn(int sockfd, struct sockaddr_storage conn_addr, socklen_t addr_size) {
  int new_fd, status;
  status = listen(sockfd, NUM_CONNECTIONS);
  if (!(status == 0)) handle_error("listen err\n");

  printf("    Now waiting for a request on port...\n");

  new_fd = accept(sockfd, (struct sockaddr*) &conn_addr, &addr_size);
  if (new_fd < 0) handle_error("accept err\n");
  return new_fd;
}

// A request will cause the server to do a switch(case) to figure out
// exactly what the response HTTP message will be. Error code handling
// will be done in their respective functions, NOT in main(),
char* process_request(char* response) {
  return response;
}

int main (int argc, char** argv) {
  struct addrinfo hints;
  struct addrinfo* serv_info;
  struct sockaddr_storage conn_addr;
  socklen_t addr_size;
  int sockfd, new_fd, bytes_read;
  char request[256];
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

  // gai_strerror()???
  status = getaddrinfo(NULL, port, &hints, &serv_info);
  if (!(status == 0)) handle_error("getaddrinfo err\n");

  sockfd = socket(serv_info->ai_family, serv_info->ai_socktype, serv_info->ai_protocol);
  if (sockfd == -1) handle_error("socket err\n");

  status = bind(sockfd, serv_info->ai_addr, serv_info->ai_addrlen);
  if (status == -1) handle_error("bind err\n");

  while (1) {
    // set up TCP connection on new_fd
    addr_size = sizeof conn_addr;
    new_fd = setup_conn(sockfd, conn_addr, addr_size);

    memset(&request, 0, sizeof(request));
    bytes_read = recv(new_fd, &request, 256, 0);
    if (bytes_read == 0) // END OF REQUESTS, TERMINATE CONNECTION
    if (bytes_read == -1) handle_error("recv err\n");

    // printf("    HTTP/1.1 request received:\n");
    // printf("    %s\n\n", request);
    // process an HTTP request
    response = process_request(request);
    printf("    %s\n\n", response);

    // send appropiate HTTP response here

    // end TCP connection
    status = close(new_fd);
    if (!(status == 0)) handle_error("close err\n");
  }

  // gai_strerror()???
  status = freeaddrinfo(serv_info);
  if (!(status == 0)) handle_error("freeaddrinfo err\n");

  return 0;
}
