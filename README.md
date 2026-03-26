# HTTP/1.1 server

> An HTTP "server" is a program that accepts connections in order to service HTTP
> requests by sending HTTP responses. (RFC9110)

This is an HTTP/1.1 server that runs over an open port on a Debian system.


### Server methods 

The minimum methods needed for a program to be HTTP complient are GET and
HEAD.

### Server responses

The server will respond with the requested content if it is able to.

### Server error codes

The server will send correct error messages if things go haywire.

### Client behavior

The example client is a simple C program that sends pre-made HTTP requests
and prints the responses to a file with the name "response{# program}.txt"

