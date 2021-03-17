/*
 *@author Joshua Naber
 *
 *
 *@title CPR E 308 Project1 UNIX Shell
 */



#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

int main(int argc, char * argv[]) {

  printf("-------------------------------\n");
  printf("Welcome To Josh's Unix Shell \n");
  printf("-------------------------------\n");

  /* Sets up Shell prompt for the user */

  char* directory = "308sh> ";
  char* addOn = "> ";
  if(argc == 3) {
    directory = argv[2];
    strcat(directory, addOn);
  } 

  /* Variables outside of the loop to work with the background process */

  pid_t backgroundPid;
  int *status;
  int backgroundProcessRunning;
  char *backgroundName;

  while(1){

    
    char command[100];
    char* betterCommand;
    char* commandParsed[20];
    char *tokenTemp;
    char *background;
    int i = 0;

    /* Resets all the commands to NULL to avoid segmentation faults */

    int b = 0;
    for(b = 0; b < 20; b++){
      commandParsed[b] = NULL;
    }
    
    /* Print Shell prompt and ask for input */

    printf(directory);
    if(!fgets(command, 100, stdin)){
      break;
    }

    /* Start strtok to parse user input, if input is just "exit", finish the program*/

    betterCommand = strtok(command, "\n");

    if(strcmp(betterCommand, "exit") == 0){
      break;
    }

    /* Check to see if any background processes have finished */

    int j = waitpid(backgroundPid,&status, WNOHANG);
    if(j > 0 && backgroundProcessRunning == 1){
      printf("Background process exit, background name is: %s\n", backgroundName);
      printf("[%d] %s Exit %d\n", backgroundPid, backgroundName, WEXITSTATUS(status));
      backgroundProcessRunning = 0;
    }

    /*Parse user input into multiple commands using strtok*/
    
    tokenTemp = strtok(betterCommand, " ");
    while(tokenTemp != NULL){
      commandParsed[i] = tokenTemp;
      tokenTemp = strtok(NULL, " ");
      i++;
    }

    /* Set variable background to last parsed command, save for later comparison and replace the & with null to avoid future errors*/ 
    background = commandParsed[i-1];

    if(strcmp(background, "&") == 0){
      commandParsed[i-1] = NULL;
    }

    /*Print the working directory using getcwd*/

    if(strcmp(commandParsed[0], "pwd") == 0){
        char currentDirectory[PATH_MAX];
	if(getcwd(currentDirectory, sizeof(currentDirectory)) != NULL){
	  printf("%s\n", currentDirectory);
	}
	else{
	  perror("Current working directory error");
	}
    }


    /*Change working Directory*/

    else if(strcmp(commandParsed[0], "cd") == 0){
      if(commandParsed[1] != NULL){
	if(chdir(commandParsed[1]) != 0){
	  perror("Change directory failed");
	}
      }
      else{
	if(chdir(getenv("HOME")) != 0){
	  perror("Change directory failed");
      }
      }
    }

    /*Return pid and ppid upon request*/

    else if(strcmp(commandParsed[0], "pid") == 0){
      printf("PID: %d\n", getpid());
    }
    else if(strcmp(commandParsed[0], "ppid") == 0){
      printf("PPID: %d\n", getppid());
    }

    /* Exectures program commands*/

    else{
      pid_t program_pid;
      int childStatus;
      program_pid = fork();
      if(program_pid < 0){
	perror("Child Creation Failed");
	exit(1);
      } 
      else if(program_pid == 0){
	execvp(commandParsed[0], commandParsed);
	perror("Child Failed To Execute");
	exit(0);
      }
      else if(strcmp(background, "&") != 0){ /* Check the background variable we made before to see if we need to wait for the process or not */
	printf("[%d] %s\n", program_pid, commandParsed[0]);
	while(wait(&childStatus) != program_pid);

	printf("[%d] %s ", program_pid, commandParsed[0]);
	printf("Exit %d\n", WEXITSTATUS(childStatus));
      }
      else{
	printf("[%d] %s\n", program_pid, commandParsed[0]); /*If it is a background command, set background variables and restart loop */
	backgroundName = commandParsed[0];
	backgroundPid = program_pid;
	backgroundProcessRunning = 1;
      }
    }

     
    
  }
  
  return 0;
}
