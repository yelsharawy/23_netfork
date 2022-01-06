#include "pipe_networking.h"

// reads a string from file descriptor `fd`
// the implementation reads an `int` as `len`,
// then reads `len+1` characters from `fd`
// if the sender did not send a terminating 0, then it won't be received either, be warned!
char *read_string(int fd) {
    int len;
    if (read(fd, &len, sizeof(int)) != sizeof(int)) return NULL;
    // len = ntohl(len);
    char *result = malloc(len+1);
    CHECKTRUEMSG(len+1 == read(fd, result, len+1), "could not read correct number of bytes");
    return result;
}

// writes a `len`-length string `str` to file descriptor `fd`
// the implementation writes `len` to `str`, then `len+1` characters of `str`
// (to send terminating 0 as well)
void write_string_len(int fd, char *str, int len) {
    // len = htonl(len);
    CHECKNEG1MSG(write(fd, &len, sizeof(int)), "could not write int");
    CHECKNEG1MSG(write(fd, str, len+1), "could not write string");
}

// shortcut for `write_string_len(fd, str, strlen(str))`
void write_string(int fd, char *str) {
    write_string_len(fd, str, strlen(str));
}


/*=========================
  server_setup
  args:

  creates the socket and binds it to a port, listening for connections

  returns the socket descriptor for incoming upstream connections
  =========================*/
int server_setup() {
  printf("[server] getting address info\n");
  struct addrinfo* results;
  //use getaddrinfo
  {
    struct addrinfo* hints;
    hints = calloc(1,sizeof(struct addrinfo));
    hints->ai_family = AF_INET;
    hints->ai_socktype = SOCK_STREAM; //TCP socket
    hints->ai_flags = AI_PASSIVE; //only needed on server
    CHECKNEG1MSG(getaddrinfo(NULL, PORTSTR, hints, &results), "getaddrinfo failed");  //Server sets node to NULL
    free(hints);
  }

  printf("[server] creating socket\n");
  //create socket
  int incoming;
  CHECKNEG1MSG(incoming = socket(results->ai_family, results->ai_socktype, results->ai_protocol), "failed to create socket");
  
  printf("[server] binding socket\n");
  CHECKNEG1MSG(bind(incoming, results->ai_addr, results->ai_addrlen), "failed to bind");
  
  printf("[server] setting socket to listen\n");
  CHECKNEG1MSG(listen(incoming, 10), "failed to listen");
  
  freeaddrinfo(results);
  return incoming;
}

/*=========================
  server_connect
  args: int incoming

  handles the subserver portion of the 3 way handshake

  returns the newly-created socket for two-way communication with this client.
  =========================*/
int server_connect(int incoming) {
  socklen_t sock_size;
  struct sockaddr_storage client_address;
  sock_size = sizeof(client_address);
  printf("[server] accepting incoming connection\n");
  int indiv;
  CHECKNEG1MSG(indiv = accept(incoming,(struct sockaddr *)&client_address, &sock_size), "failed to accept incoming connection");
  printf("[server] connected to new client!\n");
  return indiv;
}


/*=========================
  server_handshake
  args: int * to_client
  ** DO NOT USE. I don't know the implications of this
      and don't have the time or effort to think about it.
  
  Performs the server side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  int from_client = server_setup();
  *to_client = server_connect(from_client);
  return from_client;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  printf("[client] getting address info\n");
  struct addrinfo* results;
  //use getaddrinfo
  {
    struct addrinfo* hints;
    hints = calloc(1,sizeof(struct addrinfo));
    hints->ai_family = AF_INET;
    hints->ai_socktype = SOCK_STREAM; //TCP socket
    CHECKNEG1MSG(getaddrinfo("127.0.0.1", PORTSTR, hints, &results), "getaddrinfo failed");
    free(hints);
  }
  //create socket
  printf("[client] creating socket\n");
  CHECKNEG1MSG(*to_server = socket(results->ai_family, results->ai_socktype, results->ai_protocol), "failed to create socket");

  printf("[client] connecting to server\n");
  CHECKNEG1MSG(connect(*to_server, results->ai_addr, results->ai_addrlen), "failed to connect to server");
  
  return *to_server;
}
