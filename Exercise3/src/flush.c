#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#include <errno.h>
#define MAX_PATH_LENGTH 2048
#define MAX_CMD_LENGTH 4096


// Helper method to flush stdin.
void flush_in(void)
{
  int ch = 0;
  while ((ch = getchar()) != '\n' && ch != EOF);
}

void change_directory(char* buffer)
{
  int error = 0; 
  strtok(buffer, " "); // removing first token
  error = chdir(strtok(NULL, " "));
  if(error != 0)
  {    
    perror("Error");
  } 
}

int main()
{
  char* PATH = getenv("PATH"); // getting the path variable
  char  cwd[MAX_PATH_LENGTH]; //current working directory
  char  cmd[MAX_CMD_LENGTH];  //the command
  char  bfr[MAX_CMD_LENGTH];  //buffer for parsing the commnand
  char* tkn;  //buffer for storing each token
  char** tokens;
  int exit = 0;

  printf("%s/n", PATH);
  while(!exit)
  {
    getcwd(cwd, sizeof(cwd)); // getting the working directory
    printf("%s: ", cwd);
    fgets(cmd, sizeof(cmd), stdin);
    cmd[strcspn(cmd, "\r\n\0")] = 0; // removing the /n charracter
    strcpy(bfr, cmd);
    tkn = strtok(bfr, " ");
    if(strcmp(tkn, "cd") == 0)
    {
      printf("got command %s \n", cmd);
      //tkn = strtok(NULL, " ");
      //printf("%s\n", tkn);
      //chdir(tkn);
      //perror("Got this error:");
      change_directory(cmd);
    }
    else if(strcmp(cmd, "ls") == 0)
    {
      printf("got command %s \n", cmd);
    }

    else if(strcmp(cmd, "exit") == 0)
    {
      break;
    }
  }

  // Should handle

  return 0;
}
