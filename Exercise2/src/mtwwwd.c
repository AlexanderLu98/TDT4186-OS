#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include "sem.h"
#include "bbuffer.h"

#define MAXREQ (4096*1024)

//char buffer[MAXREQ], body[MAXREQ], msg[MAXREQ];
void error(const char *msg){perror(msg); exit(1);}


int main(int argc, char *argv[])
{
  // initializing the path as a string with max length of 100, because that should be enough.
  const char www_path_head[1024];
  int port;
  int num_of_threads;
  int num_of_buffer_slots;
  BNDBUF *bb;

  // DEBUG AREA -- JUST FOR TESTING!
  /*
  printf("starting init!\n");
  BNDBUF *bb = bb_init(10);
  printf("starting add!\n");
  bb_add(bb, 20);
  printf("got past add!\n");
  printf("%d \n", bb_get(bb));
  return 0;
  */
  // DEBUG AREA -- JUST FOR TESTING!

  // Parse arguments
  if (argc > 1)
  {
    strcpy(www_path_head, argv[1]);
  }
  else
  {
    error("No www-path given, terminating application \n");
  }
  if (argc > 2)
  {
    sscanf(argv[2], "%d", &port);
  }
  else
  {
    error("No port given, terminating appication \n");
  }
  if (argc > 3)
  {
    sscanf(argv[3], "%d", &num_of_threads);
  }
  if (argc > 4)
  {
    sscanf(argv[4], "%d", &num_of_buffer_slots);
  }

  printf("Program started with port %i and www_path_head: %s \n", port, www_path_head);

  int sockfd, connfd;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;

  // buffer
  bb = bb_init(num_of_buffer_slots);
  // list of all threads:
  pthread_t threads[num_of_threads];
  // initialize the threads

  void *serve_request()
  {
    int connfd, n;
    char *www_path, *buffer, *body, *msg;
    www_path = malloc(sizeof(char)*MAXREQ);
    buffer   = malloc(sizeof(char)*MAXREQ);
    body     = malloc(sizeof(char)*MAXREQ);
    msg      = malloc(sizeof(char)*MAXREQ);
    FILE *fp;

    //strcpy(www_path, "Yo\n");

    printf("Got to before the while loop: %s \n", www_path_head);
    while(1)
    {
      printf("Checkpoint 0!\n");
      connfd = bb_get(bb);
      printf("Checkpoint 1!\n");
      n = read(connfd, buffer, sizeof(buffer) - 1);
      printf("Checkpoint 2!\n");
      if(n < 0) error("Error reading from socket");


      printf("Checkpoint 3!\n");
      //printf("Before: %s\n", www_ph);
      strcpy(www_path, www_path_head);
      printf("After : %s\n", www_path);
      // ignores the first token after " "
      strtok(buffer, " ");
      // concatenates the second token to www_path
      strcat(www_path, strtok(NULL, " "));
      // opens the requested file
      fp = fopen("/home/hallvard/tdt4186/TDT4186-OS/Exercise2/pages/index.html","r");

      if (fp != NULL)
      {
        printf("Checkpoint 4!\n");
        size_t newLen = fread(body, sizeof(char), MAXREQ, fp);

        if ( ferror(fp) != 0 )
        {
          fputs("Error reading file\n", stderr);
          strcpy(body, "404 no such file!\n");
          snprintf(msg, sizeof(msg),
                    "HTTP/1.0 200 OK\n"
                    "Content-Type: text/html\n"
                    "Content-Length: %d\n\n%s",
                    strlen(body), body);
          n = write(connfd, msg, strlen(msg));
        }
        else
        {
          body[newLen++] = '\0';
          snprintf(msg, sizeof(msg),
                    "HTTP/1.0 200 OK\n"
                    "Content-Type: text/html\n"
                    "Content-Length: %d\n\n%s",
                    strlen(body), body);
          n = write(connfd, msg, strlen(msg));
          if(n < 0) error("Error writing to socket");
        }
        fclose(fp);
      }
      close(connfd);
    }
    free(www_path);
    free(buffer);
    free(body);
    free(msg);
  }


  //initializing threads.
  for(int i=0; i<num_of_threads; i++)
  {
      pthread_create(&threads[i], NULL, serve_request, NULL);
  }

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1) error("Error opening socket \n ");
  printf("Socket successfull created \n");
  bzero((char *) &serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);

  if(bind(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) error("Error binding socket \n");
  printf("Socket successfull bound \n");

  if(listen(sockfd, 5) !=0) error("Error listening to socket \n");
  printf("Server listening \n");

  while(1)
  {
    clilen = sizeof(cli_addr);
    connfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if(connfd < 0) error("Error on accept \n");
    printf("Connection accepted \n");
    bb_add(bb, connfd);
  }

  // cleaning up by deleting bounded buffer
  bb_del(bb);
}
