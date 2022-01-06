#include <signal.h>
#include "pipe_networking.h"


void sighandler(int signum) {
  printf("\nCleaning up...\n");
  // unlink("mario");
  // ^ we don't care whether this succeeds; what more could be done if it fails?
  // besides, `mario` may not exist if server is already connected with a client
  exit(0);
}

// in-place ROT13
// tried to optimize as much as possible just for fun
// didn't actually test the speed, so i don't know how much using bit operations helped
// as opposed to regular math and comparisons
void transform_string(char *str) {
    while (*str) {
        unsigned char div, rem;
        if ((div = *str >> 5) & 2 && (rem = ((*str) & 31) - 1) < 26)
            *str = (div << 5) | ((rem + 13) % 26 + 1);
        str++;
    }
}

int main() {
  
  signal(SIGINT, sighandler);
  signal(SIGTERM, sighandler);

  int indiv;
  int incoming;
  
  int child_counter = 0;
  
  incoming = server_setup();
  
  // main loop
  while (1) {
    // finish handshake
    indiv = server_connect(incoming);
    int childpid;
    if (!(childpid = fork())) {
      // CHILD
      // client interaction loop
      while (1) {
        printf("[Child %d] Waiting for client input...\n", child_counter);
        char *message = read_string(indiv);
        if (message == NULL) {
          printf("[Child %d] Client disconnected.\n", child_counter);
          close(indiv);
          return 0;
        }
        printf("[Child %d] Received from client: \"%s\"\n", child_counter, message);
        transform_string(message);
        printf("[Child %d] Responding to client: \"%s\"\n", child_counter, message);
        write_string(indiv, message);
      }
    }
    child_counter++;
    // cleanup from last client to be ready for new client
    CHECKNEG1MSG(close(indiv), "[server] failed to close newly created socket");
  }
}
