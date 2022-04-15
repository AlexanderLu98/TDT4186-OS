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

// struct for linked list for active processes
typedef struct AProcess
{
  struct AProcess* prev;
  struct AProcess* next;
  int PID;
  char* cmd;
}AProcess; 


struct AProcess* init_AProcess(int PID, char* cmd)
{
  AProcess* aproc = malloc(sizeof(AProcess));
  aproc -> next = NULL; 
  aproc -> PID  = PID;
  aproc -> cmd  = malloc(sizeof(char)*MAX_CMD_LENGTH); 
  strcpy(aproc -> cmd, cmd);
  return aproc; 
}

struct AProcess* del_AProcess_by_PID(AProcess* head, int PID)
{
  // Deletes the element in the list matching the PID
  // Does not guard against this being the head of the list.  
  AProcess* tmp = head;
  AProcess* aproc = tmp -> next;

  if(head -> PID == PID)
  {
    free(head -> cmd);
    free(head);
    return aproc; //returns the next element in the list as the head 
  }

  while(aproc != NULL)
  {
    if(aproc -> PID == PID)
    {
      tmp -> next = aproc -> next; 
      free(&aproc -> cmd);
      free(aproc);
      break;
    }
    else
    {
      tmp = aproc; 
      aproc = aproc -> next; 
    }
  }
  return head; 
}

struct AProcess* get_AProcess_by_PID(AProcess* head, int PID)
{
  AProcess* aproc = head; 
  while(aproc != NULL)
  {
    if(aproc -> PID == PID)
    {
      return aproc; 
    }
    aproc = aproc -> next; 
  }
  return NULL; 
}


// Goes through the linked list and deletes all elements 
void del_AProcess_list(AProcess** head)
{ 
  while(*head != NULL)
  {
    AProcess* tmp = *head;
    *head = (*head)-> next;
    free(tmp->cmd);
    free(tmp);  
  }
}

// prints all the processes in the linked list 
void print_AProcess_list(AProcess* head)
{
  while(head != NULL)
  {
    printf("Process ID:[%d] command:[%s]\n", head -> PID, head -> cmd);
    head = head -> next;
  }
}

// inserts a node at the tail of the list
void insert_AProcess(AProcess* head, AProcess* aproc)
{
  AProcess* node  = head;

  while(node -> next != NULL)
  {
    node = node -> next; 
  } 
  node -> next = aproc; //inserts the node as the last element.  
}

// Helper method to flush stdin.
void flush_in(void)
{
  int ch = 0;
  while ((ch = getchar()) != '\n' && ch != EOF);
}

void make_null(char** buffer, int length) 
{ 
  // sets all values in a buffer to NULL
  for(int i = 0; i < length; i++)
  {
    buffer[i] = NULL;
  }
}

int read_from_file(FILE* fp, char** buffer)
{
  int error = 1;
  return error;
}

int write_to_file(FILE* fp, char** buffer)
{
  int error = 1; 
  return error;
}

// Change directory operation returns an error code if an error occurs.
int change_directory(char** args)
{
  int error = 1;

  if(args[1] != NULL)
  {
    error = chdir(args[1]);
    if(error != 0)
    {
      perror("cd failed with error");
    }
  }
  else printf("cd: no argument received. \n");
  
  return error; 
}

// Executes commands possibly found in the path. Return error if not sucessful. 
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

// takes in a string tokenizes it based on sep, and stores it in buffer 
int tokenize(char* str, char* sep, char** buffer)
{
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
  char  cwd[MAX_PATH_LENGTH];  // current working directory
  char  cmd[MAX_CMD_LENGTH];   // the command
  char  cbf[MAX_CMD_LENGTH];   // command buffer
 
  int cmd_len; 
  int bgtask;  // background task

  char* paths[MAX_ARG_COUNT] = {};
  char* args[MAX_ARG_COUNT] = {};
  tokenize(PATH, ":", paths);

  int pid; 
  int exit_status;
  int dead_PID; 
  AProcess* head = init_AProcess(-1, "head");

  while(1)
  { 
    getcwd(cwd, sizeof(cwd)); // getting the working directory
    printf("%s: ", cwd);
     
    if(fgets(cmd, sizeof(cmd), stdin) == NULL) // EOF by control-d
    {
      printf("Exiting program.\n");
      while(head != NULL)
      {
        del_AProcess_list(&head);  
      }
      exit(0);
    }

    cmd_len = strlen(cmd);
    
    if(strspn(cmd, " \r\n\t") == cmd_len)    // handles empty strings and strings that are just spaces 
    {
      continue; 
    } 
   
    bgtask = (cmd[cmd_len-2] == '&') ? 1 : 0;     // checks if the last character of the command is '&'  
    cmd[strcspn(cmd, "&\r\n")] = '\0';            // removes new line chars and '&'.
    strcpy(cbf, cmd);                             // buffer to store cmd before tokenizing 
    tokenize(cmd, " \t", args);                   // deliminating by space and tab.

    if(strcmp(args[0], "cd") == 0)
    {
      exit_status = change_directory(args);
      printf("Exit staus [ %s ] = %d \n", cbf, exit_status);
    }
    else if(strcmp(args[0], "jobs") == 0)
    {
      print_AProcess_list(head);
    }
    else if(bgtask == 1)
    {
      pid = _fork(paths, args);
      AProcess* aproc = init_AProcess(pid, cbf);
      insert_AProcess(head, aproc);
      printf("Executing background task: [ %s ] \n", cbf);
    }
    else
    {
      pid = _fork(paths, args);
      dead_PID = waitpid(pid, &exit_status, 0); 
      printf("Exit staus [ %s ] = %d \n", cbf, exit_status);
    }

    dead_PID = waitpid(-1, &exit_status, WNOHANG); 
    if(dead_PID > 0)
    {
      AProcess* DAProc = get_AProcess_by_PID(head, dead_PID);
      printf("Exit status: %d cmd:[%s], PID: %d. \n", exit_status, DAProc->cmd, DAProc->PID);
      head = del_AProcess_by_PID(head, dead_PID);
    }
    make_null(args, MAX_ARG_COUNT); 
  }
  return 0;
}

// Command line redirection
/*
 * TODO: 3.3 The command will need to be tokenized in two operations. The < operator can bee treated as 
 *           token when seperated by spaces. The io redicretion only servers to read and write to files, 
 *           and therefore the operation is less complicated than the pipe. The input is either passes 
 *           as an argument to the bash file, or the output is passed as an argument to a 
 *           write operation to a text file.
 *
 * TODO: 3.4 Should be pretty simply done with an if else clause at the end,
 *           the forground task should also catch zombies though. A linked list with appropriate 
 *           structs should keep the pids in order. 
 *           When a task is complete the linked list element is delted and its memory cleared 
 *
 * TODO: 3.5 Walk the linked list and print out the PID and cmd. Same cmd structure as cd. 
 *
 * /
 * */


