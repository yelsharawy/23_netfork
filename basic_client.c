#include "pipe_networking.h"


int main() {

  int to_server;
  int from_server;

  from_server = client_handshake( &to_server );
  
  printf("! You can use Ctrl+C to quit this process.\n"
         "! Alternatively, press Ctrl+D to end your input.\n"
         "! Either way works the same.\n");
  while (1) {
      char *line = NULL;
      unsigned long allocatedLen = 0;
      printf("Input string to process: ");
      fflush(stdout);
      int actualLen = getline(&line, &allocatedLen, stdin); // allocates `line` for me :)
      if (actualLen == -1) {
          printf("\nCould not read from stdin. Will exit now.\n");
          free(line);
          return 0;
      }
      if (line[actualLen-1] == '\n') line[actualLen-1] = 0;
      else printf("\n");
      // * known bug: this seems to terminate the program if the other end has closed
      // not sure why this happens? i don't even get a segfault
      write_string_len(to_server, line, actualLen);
      free(line);
      
      char *received = read_string(from_server);
      if (received == NULL) {
          printf("No string received from server - killed early? Will exit now.\n");
          return -1;
      }
      printf("Received from server: \"%s\"\n", received);
      free(received);
  }
}
