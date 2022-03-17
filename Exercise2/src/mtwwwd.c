#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define MAXREQ (4096*1024)

char buffer[MAXREQ], body[MAXREQ], msg[MAXREQ];
void error(const char *msg){perror(msg); exit(1);}

int main(int argc, char *argv[])
{
  // initializing the path as a string with max length of 100, because that should be enough.
  char www_path[MAXREQ];
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
    error("No www-path given, terminating aplication \n");
  }
  if (argc > 2)
  {
    sscanf(argv[2], "%d", &port);
  }
  else
  {
    error("No port given, terminating aplication \n");
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

  int sockfd, connfd, n;
  socklen_t clilen;
  struct sockaddr_in serv_addr, cli_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd == -1){error("Error opening socket \n ");}
  printf("Socket successfull created \n");
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(port);

  if(bind(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){error("Error binding socket \n");}
  printf("Socket successfull bound \n");

  if(listen(sockfd, 5) !=0){error("Error listening to socket \n");}
  printf("Server listening \n");

  while(1)
  {
    clilen = sizeof(cli_addr);
    connfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if(connfd < 0){error("Error on accept \n");}
    printf("Connection accepted \n");
    bzero(buffer, sizeof(buffer));
    n = read(connfd, buffer, sizeof(buffer) - 1);
    if(n < 0) error("Error reading from socket");
    snprintf(body, sizeof(body),
             "<html>\n<body>\n"
             "<h1> Hello web browser</h1>\nYour request was\n"
             "<pre>%s</pre>\n"
             "</body>\n</html>\n", buffer);
    snprintf(msg, sizeof(msg),
             "HTTP/1.0 200 OK\n"
             "Content-Type: text/html\n"
             "Content-Length: %d\n\n%s",
             strlen(body), body);
    n = write(connfd, msg, strlen(msg));
    if(n < 0) error("Error writing to socket");
    close(connfd);
  }
}
