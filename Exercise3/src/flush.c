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
#define MAX_ARG_COUNT 30

// struct for linked list
typedef struct AProcess
{
  struct AProcess* prev;
  struct AProcess* next;
  int PID;
  char* cmd;

}AProcess; 


struct AProcess* init_AProc(AProcess* next, AProcess* prev, int PID, char* cmd)
{
  AProcess* aproc = malloc(sizeof(AProcess));
  aproc -> next = next; 
  aproc -> prev = prev;
  aproc -> PID  = PID;
  aproc -> cmd  = cmd; 

  return aproc; 
}

void del_AProc(AProcess* aproc)
{
  if(aproc -> prev != NULL)
  {
    aproc -> prev -> next = aproc -> next;
  }
  if(aproc -> next != NULL)
  {
    aproc -> next -> prev = aproc -> prev;  
  }
  free(aproc);
}

void del_linked_AProc(AProcess* aproc)
{
  AProcess* head = aproc -> prev;
  AProcess* next = aproc -> next;

  while(head -> prev != NULL) //iterates through list to find the head
  {
    head = head->prev; 
  }

  do
  {
    next = head -> next;
    del_AProc(head);
    head = next; 
  }
  while(next != NULL);
}


// Helper method to flush stdin.
void flush_in(void)
{
  int ch = 0;
  while ((ch = getchar()) != '\n' && ch != EOF);
}


void make_null(char** buffer, int length) 
{
  for(int i = 0; i < length; i++)
  {
    buffer[i] = NULL;
  }
}

void change_directory(char** args)
{
  int error = 0;

  if(args[1] != NULL)
  {
    error = chdir(args[1]);
    if(error != 0)
    {
      perror("cd failed with error");
    }
  }
  else printf("cd: no argument received. \n");
}


int execute_command(char** paths, char** args)
{
  int error; // if executing the command was sucessfull or not
  char cmd_pth[MAX_PATH_LENGTH];
  
  if(access(args[0], F_OK) == 0)
  {
    strcpy(cmd_pth, args[0]);
  }
  else
  {
    for(int i = 0; (paths[i] != NULL) && i < MAX_ARG_COUNT; i++)
    {
      snprintf(cmd_pth, MAX_PATH_LENGTH, "%s/%s", paths[i], args[0]);
      if(access(cmd_pth, F_OK) == 0)
      {
        break; // The file we are searching for has been found!
      }
    }
  }
  // lets execv handle the error messagin even when file not found
  error = execv(cmd_pth, args);
  return error; 
}

int tokenize(char* str, char* sep, char** buffer)
{
  // takes in a string tokenizes it based on sep, and stores it in buffer 
  int counter = 0;
  int status = 0; 

  for(char* tkn = strtok(str, sep); tkn != NULL; tkn = strtok(NULL, sep))
  {
    if(counter > MAX_ARG_COUNT)
    {
      printf("Unable to parse string, MAX_ARG_COUNT exceded. \n");
      status = 1;
      break; 
    }
    buffer[counter] = tkn;
    counter ++;
  }
  return status; 
}

// helper method for using fork
unsigned int _fork(char** paths, char** args)
{
  unsigned int pid = fork();

  if(pid != 0)
  { // Parent
    return pid;
  }
  else
  {
    int error = 0; 
    error = execute_command(paths, args); 
    if(error != 0)
    {
      perror("Following error occured executing command");
    }
    //clean up and exiting the process
    exit(pid);
    return 0;
  }
}


int main()
{ 
  char* PATH = getenv("PATH"); // getting the path variable
  char  cwd[MAX_PATH_LENGTH];  //current working directory
  char  cmd[MAX_CMD_LENGTH];   //the command
 
  char* paths[MAX_ARG_COUNT] = {};
  char* args[MAX_ARG_COUNT] = {};
  tokenize(PATH, ":", paths);

  /*
  for(int i; i<MAX_ARG_COUNT; i++)
  {
    if(pth_tkn[i] != NULL)
    {
      printf("%s\n", pth_tkn[i]);
    }
  }
  */
  
  int pid; 

  while(1)
  { 
    getcwd(cwd, sizeof(cwd)); // getting the working directory
    printf("%s: ", cwd);
    
    make_null(args, MAX_ARG_COUNT); 
    if(fgets(cmd, sizeof(cmd), stdin) == NULL) // EOF by control-d
    {
      printf("Exiting program.\n");
      exit(0);
    }
    cmd[strcspn(cmd, "\r\n")] = 0;
    tokenize(cmd, " \t", args); // deliminating by space and tab.  
    if(strcmp(args[0], "cd") == 0)
    {
      change_directory(args);
    }
    else
    {
      pid = _fork(paths, args);
      waitpid(pid, NULL, 0);
    }
  }

  // Should handle
        
  return 0;
}


// Command line redirection
/*
 * TODO: 3.3 the command will need to be tokenized in two operations. The < operator can bee treated as 
 *           token when seperated by spaces. The io redicretion only servers to read and write to files, 
 *           and therefore the operation is less complicated than the pipe. The input is either passes as 
 *           an argument to the bash file, or the output is passed as an argument to a write operation to 
 *           a text file. 
 * TODO: 3.4 Should be pretty simply done with an if else clause at the end,
 *           the forground task should also catch zombies though. A linked list with appropriate 
 *           structs should keep the pids in order. 
 *           When a task is complete the linked list element is delted and its memory cleared 
 * TODO: 3.5 Get all children of the parrent process and find them in the linked list. 
 *
 * /
 * */


