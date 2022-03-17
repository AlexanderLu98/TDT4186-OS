#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int main(int argc, char *argv[])
{
  // initializing the path as a string with max length of 100, because that should be enough.
  char www_path[100];
  int port;
  int num_of_threads;
  int num_of_buffer_slots;
  // Parse arguments
  if (argc > 1)
  {
    strcpy(www_path, argv[1]);
  }
  else
  {
    printf("No www-path given, terminating aplication \n");
    return 0;
  }
  if (argc > 2)
  {
    sscanf(argv[2], "%d", &port);
  }
  else
  {
     printf("No port given, terminating aplication \n");
     return 0;
  }
  if (argc > 3)
  {
    sscanf(argv[3], "%d", &num_of_threads);
  }
  if (argc > 4)
  {
    sscanf(argv[4], "%d", &num_of_buffer_slots);
  }

  printf("Program started with port %i and www_path: %s \n", port, www_path);

  // Open port to requests

  // establish connection

  // while the connection is open start serving the requests that it gets:

  // soccet
  // bind
  // listen

}
