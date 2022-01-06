#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#ifndef NETWORKING_H
#define NETWORKING_H

// my error-checking macros
#define CHECKTRUEMSG(value, msg) do {       \
    if (!(value)) {                         \
        printf("ERROR: "msg"\n");           \
        exit(-1);                           \
    }                                       \
} while(0);

#define CHECKNEG1MSG(value, msg) CHECKTRUEMSG((value) != (typeof(value))-1, msg": %m")

// unused
#define ACK "HOLA"
#define WKP "mario"

#define PORT 18588
#define STRINGIFY(s) #s
#define TOSTRING(s) STRINGIFY(s)
#define PORTSTR TOSTRING(PORT)

#define HANDSHAKE_BUFFER_SIZE 10
#define BUFFER_SIZE 1000


//for forking server
int server_setup();
int server_connect(int from_client);


int server_handshake(int *to_client);
int client_handshake(int *to_server);

// my helper functions
char * read_string(int fd);
void write_string_len(int fd, char *str, int len);
void write_string(int fd, char *str);

int server_handshake(int *to_client);
int client_handshake(int *to_server);

#endif
