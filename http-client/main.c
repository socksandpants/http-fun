#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/*  Author: Ethan Jones
*   Date: 2025-12-27
*   Program: http-client
*
*   Resources used:
*     - RFCs 3986, 9110, 9112
*     - ISO/IEC9899:2017
*     - Beej's guide to network programming (for networking brushing up)
*/

#define handle_error(msg) \
  do { perror(msg); exit(EXIT_FAILURE); } while(0)

int main (int argc, char** argv) {
  char port;
  char* address;


  if (argc != 3) { handle_error("give a port and an address.\n"); }

  port = atoi(argv[1]);
  address = argv[2];

  // HTTP/1.1 connections are "persistent," what this actually means is parallel sending of
  // requests and reponses. For the client it is only concerned with requests. SEE rfc9112.9.3



  return 0;
}
