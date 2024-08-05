#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "cmds.h"

#define STDIN  0
#define STDOUT 1
#define STDERR 2

#define BUFF_SIZE 200

int main(void)
{
	// Initialize shell
	char command[BUFF_SIZE];
	int readSize = 0;
	const char * shellMsg = ": Etfadal> ";

	// phist init
	char hist[10][200];
	int st[10];
	int c = 0;

	char **argv;
	int argc;

	while (1)
	{
		// Prompt & get input
		getcwd(command, BUFF_SIZE);
		write(STDOUT, command, strlen(command));

		write(STDOUT, shellMsg, strlen(shellMsg));
		readSize = read(STDIN, command, BUFF_SIZE);
		command[readSize - 1] = '\0';

		// Handle piping
		int numPipes = countArgs(strdup(command), "|");
		char** pipes = getArgs(command, numPipes, "|");
		
		int pipefd1[2];
		int pipefd2[2];

		int isExit = 0;
		
		
		for (int i = 0; i < numPipes; i++)
		{
			// Parsing command
			argc = countArgs(pipes[i], " ");
			if (argc > 0) argv = getArgs(pipes[i], argc, " ");
			else break;
			
			// Check for exit before fork
			if (strcmp(argv[0], "exit") == 0)
			{
				isExit = 1;
			       	break;
			}	
			
			// Close open pipes and open new pipe
			if (numPipes > 1)
			{
				if (i == 0)
				{
					pipe(pipefd1);	
				}
				else if (i == numPipes - 1)
				{
					if (i % 2 == 0) close(pipefd2[1]);
					else            close(pipefd1[1]);
				}
				else
				{
					if (i % 2 == 0)
					{
						pipe(pipefd1);
						close(pipefd2[1]);
					}
					else
					{
						pipe(pipefd2);
						close(pipefd1[1]);
					}
				}
			}
		
			// Run command
			int ret = fork();
			if (ret > 0)
			{
				int status;
				wait(&status);
				addphist(hist, st, argv[0], status, &c);
			}
			else if (ret == 0) 
			{
				// Alternate between pipes
				if (numPipes > 1)
				{
					if (i == 0)
					{
						dup2(pipefd1[1], STDOUT);
					}
					else if (i == numPipes - 1)
					{

						if (i % 2 == 0) dup2(pipefd2[0], STDIN);
						else            dup2(pipefd1[0], STDIN);
					}
					else
					{
						if (i % 2 == 0)
						{
							dup2(pipefd2[0], STDIN);
							dup2(pipefd1[1], STDOUT);
						}
						else
						{
							dup2(pipefd1[0], STDIN);
							dup2(pipefd2[1], STDOUT);
						}
					}
				}

				// Check commands
				if      (strcmp(argv[0], "mypwd") == 0)     getpwd();
				else if (strcmp(argv[0], "myecho") == 0)    echo(command + 7, argc);
				else if (strcmp(argv[0], "mycp") == 0)      cpy(argc, argv);
				else if (strcmp(argv[0], "mymv") == 0)      mv(argc, argv);
				else if (strcmp(argv[0], "myhelp") == 0)    help();
				else if (strcmp(argv[0], "cd") == 0)        cd(argc, argv);
				else if (strcmp(argv[0], "envir") == 0)     envir(argv[1]);
				else if (strcmp(argv[0], "type") == 0)      type(argv[1]);
				else if (strcmp(argv[0], "phist") == 0)     phist(hist, st, &c);
				else if (checkExtern(argv[0], command) > 0) runExtern(argv, command);
				else                                        write(STDOUT, "Command not found.\n", 19), exit(1);
				
				exit(0);
			}
			else    perror("Fork");
				
			// Free up memory in argv
			for (int i = 0; i < argc; i++) free(argv[i]);
			free(argv);
		}

		// Free up pipes
		for (int i = 0; i < numPipes; i++) free(pipes[i]);
		free(pipes);

		if (isExit) break;
	}

	// Exit message
	strcpy(command, "Farewell :(\n");
	write(STDOUT, command, strlen(command));

	return 0;
}


















