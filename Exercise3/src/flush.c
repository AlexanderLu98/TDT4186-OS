#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <limits.h>
#define MAX_PATH_LENGTH 2048
#define MAX_CMD_LENGTH 4096

// Helper method to flush stdin.
void flush_in(void)
{
  int ch = 0;
  while ((ch = getchar()) != '\n' && ch != EOF);
}

int main()
{
  char cwd[MAX_PATH_LENGTH]; //current working directory
  char cmd[MAX_CMD_LENGTH];  //the command
  char bfr[MAX_CMD_LENGTH];  //buffer for parsing the commnand
  char tkn[MAX_CMD_LENGTH];  //buffer for storing each token
  int exit = 0;

  getcwd(cwd, sizeof(cwd)); // getting the working directory

  while(!exit)
  {
    printf("%s: ", cwd);
    fgets(bfr, sizeof(cmd), stdin);
    strcpy(cmd, strtok(bfr, " "));
    if(strcmp(cmd, "cd\n") == 0)
    {
      printf("got command %s", cmd);
    }
    else if(strcmp(cmd, "ls\n") == 0)
    {
      printf("got command %s", cmd);
    }

    else if(strcmp(cmd, "exit\n") == 0)
    {
      break;
    }
  }

  // Should handle

  return 0;
}
