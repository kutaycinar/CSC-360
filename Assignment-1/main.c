/*
 *Kutay Cinar - V00******
 *CSC360 - Assignment 1
 *A simple shell program
 *created: June 7 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// *************************************************
// Struct for storing process fields
// *************************************************

struct process
{
	pid_t pid;
	char command[15];	// assumption 1: The maximum number of parameters to any command is less than 15
	char status;
};

// *************************************************
// Global variables
// *************************************************

struct process bglist[6] = {};	// background process list (index 0 not used)
int bgcount = 0;	// background process count

// *************************************************
// Part I - Basic execution
// *************************************************

// Run basic command
void execute(char **commands)
{
	pid_t pid = fork();	// uses fork to return twice

	if (pid < 0)
	{
		perror("fork failed.");
		exit(-1);
	}
	else if (pid == 0)	// child process
	{
		execvp(commands[0], commands);
		perror("execvp failed.");
		exit(-1);
	}
	else	// parent process
	{
		waitpid(pid, NULL, 0);
	}
}

// *************************************************
// Part II – Changing Directories 
// *************************************************

// Change directory (cd) command
void changeDirectory(char *directory)
{

	if (strcmp(directory, "~") == 0)	// move to home directory
	{
		if (getenv("HOME"))
		{
			char *home = getenv("HOME");
			if (chdir(home) != 0)
			{
				perror("chdir failed.");
				exit(-1);
			}
		}
		else
		{
			perror("getenv(\"HOME\") is not set.");
			exit(-1);
		}
	}
	else if (strcmp(directory, "..") == 0)	// move one up directory
	{
		if (chdir("../") != 0)
		{
			perror("chdir failed.");
			exit(-1);
		}
	}
	else
	{
		if (chdir(directory) != 0)
		{
			perror("chdir failed.");
			exit(-1);
		}
	}
}

// List parent working directory (pwd) command
void parentWorkingDirectory(void)
{

	char cwd[256];	// assumption 2: The length of the current working directory is less than 256 characters

	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("getcwd failed.");
		exit(-1);
	}
	else
	{
		printf("%s", cwd);
	}
}

// *************************************************
// Part III - Background Execution
// *************************************************

// Run command in the background
void executeBackground(char **commands)
{
	pid_t pid = fork();	// uses fork to return twice

	if (pid < 0)
	{
		perror("fork failed.");
		exit(-1);
	}
	else if (pid == 0)	// child process
	{
		execvp(commands[1], commands + 1);
		perror("execvp failed.");
		exit(-1);
	}
	else	// parent process
	{
		bgcount++;
		bglist[bgcount].pid = pid;
		strncpy(bglist[bgcount].command, commands[1], 15);
		bglist[bgcount].status = 'R';
	}
}

// Background process deletion
void deleteProcess(pid_t pid)
{
	for (int i = 1; i < 6; i++)
	{
		if (bglist[i].pid == pid)
		{
			bglist[i].pid = 0;
			bgcount--;
		}
	}
}

// Reordering background array list
void reorderBackgroundList(void)
{
	for (int i = 1; i < 5; i++)
	{
		if (bglist[i].pid == 0)
		{
			bglist[i] = bglist[i + 1];
			bglist[i + 1].pid = 0;
		}
	}
	bglist[5].pid = 0;	// unset last element edge case
}

// Listing background processes
void listBackground(void)
{
	int count = 0;
	for (int i = 1; i < 6; i++)
	{
		if (bglist[i].pid != 0)
		{
			//printf("%d[%c]: %s pid: %d\n",i-1, bglist[i].status, bglist[i].command, bglist[i].pid);
			printf("%d[%c]: ", i - 1, bglist[i].status);
			parentWorkingDirectory();
			printf("/%s\n", bglist[i].command);
			count++;
		}
	}
	printf("Total Background jobs: %d\n", count);
}

// Updating background processes (killing zombie processes)
void updateProcesses()
{
	if (bgcount > 0)
	{
		pid_t pid = waitpid(0, NULL, WNOHANG);

		while (pid > 0)
		{
			// loop for if multiple pids are hanging

			deleteProcess(pid);
			reorderBackgroundList();

			pid = waitpid(0, NULL, WNOHANG);
		}
	}
}

// Kill a background job
void backgroundKill(int processNumber)
{

	pid_t pid = bglist[processNumber].pid;

	if (kill(pid, SIGKILL) != 0)
	{
		perror("SIGKILL failed.");
		exit(-1);
	}
	else
	{
		printf("Process killed.\n");
	}
}

// *************************************************
// Part IV - Suspending and Resuming background jobs
// *************************************************

// Suspend background job
void backgroundStop(int processNumber)
{

	pid_t pid = bglist[processNumber].pid;

	if (bglist[processNumber].status == 'S')
	{
		printf("Cannot stop a suspended process.\n");
	}
	else
	{
		if (kill(pid, SIGSTOP) != 0)
		{
			perror("SIGSTOP failed.");
			exit(-1);
		}
		else
		{
			bglist[processNumber].status = 'S';
			printf("Process suspended.\n");
		}
	}
}

// Resume background job
void backgroundStart(int processNumber)
{

	pid_t pid = bglist[processNumber].pid;

	if (bglist[processNumber].status == 'R')
	{
		printf("Cannot resume a running process.\n");
	}
	else
	{
		if (kill(pid, SIGCONT) != 0)
		{
			perror("SIGCONT failed.");
			exit(-1);
		}
		else
		{
			bglist[processNumber].status = 'R';
			printf("Process resumed.\n");
		}
	}
}

// *************************************************
// Main
// *************************************************
int main(void)
{
	for (;;)
	{
		parentWorkingDirectory();	// get parent working directory with no parameters
		
		char *cmd = readline(">");

		char *commands[15];	// assumption 1: The maximum number of parameters to any command is less than 15
		char *args = strtok(cmd, " ");

		if (*cmd != '\0')	// check that we are given input for our shell
		{
			// Split user input into commands
			int i = 0;
			while (args)
			{
				commands[i++] = args;
				args = strtok(NULL, " ");
			}
			commands[i] = NULL;

			// Update background array list (handles zombie processes)
			updateProcesses();

			// Internal shell commands
			if (strcmp(commands[0], "pwd") == 0)
			{
				parentWorkingDirectory();	// pwd
				printf("\n");
			}
			else if (strcmp(commands[0], "cd") == 0)
			{
				changeDirectory(commands[1]);	// cd

			}
			else if (strcmp(commands[0], "bg") == 0)
			{
				if (bgcount < 5)	// assumption 3: There will be at most 5 background jobs
				{
					executeBackground(commands);	// bg
				}
				else
				{
					printf("This program only supports up to 5 background jobs.\n");
				}
			}
			else if (strcmp(commands[0], "bglist") == 0)
			{
				listBackground();	// bglist
			}
			else if (strcmp(commands[0], "bgkill") == 0)
			{
				if (commands[1] != NULL)
				{
					int processNumber = atoi(commands[1]) + 1;	// add 1 as index 0 is not used in bglist

					if (0 < processNumber && processNumber <= bgcount)
					{
						backgroundKill(processNumber);	// kill
					}
					else
					{
						printf("Please put in a running process ID.\n");
					}
				}
				else
				{
					printf("Please put in a process ID in range 0-4.\n");
				}
			}
			else if (strcmp(commands[0], "stop") == 0)
			{
				if (commands[1] != NULL)
				{
					int processNumber = atoi(commands[1]) + 1;	// add 1 as index 0 is not used in bglist

					if (0 < processNumber && processNumber <= bgcount)
					{
						backgroundStop(processNumber);	// stop
					}
					else
					{
						printf("Please put in a running process ID.\n");
					}
				}
				else
				{
					printf("Please put in a process ID in range 0-4.\n");
				}
			}
			else if (strcmp(commands[0], "start") == 0)
			{
				if (commands[1] != NULL)
				{
					int processNumber = atoi(commands[1]) + 1;	// add 1 as index 0 is not used in bglist

					if (0 < processNumber && processNumber <= bgcount)
					{
						backgroundStart(processNumber);	// start
					}
					else
					{
						printf("Please put in a running process ID.\n");
					}
				}
				else
				{
					printf("Please put in a process ID in range 0-4.\n");
				}
			}
			else if (strcmp(commands[0], "exit") == 0)
			{
				free(cmd);
				exit(0);	// exit
			}
			else
			{
				execute(commands);	// if no internal command, run basic command
			}
		}
		free(cmd);
	}
	return 0;
}