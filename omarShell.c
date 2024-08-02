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

		// Parsing command
		argc = countArgs(command);
		if (argc > 0) argv = getArgs(command, argc);
		else continue;	

		// Check commands
		if      (strcmp(argv[0], "mypwd") == 0)   getpwd();
		else if (strcmp(argv[0], "myecho") == 0)  echo(command + 7, argc);
		else if (strcmp(argv[0], "mycp") == 0)    cpy(argc, argv);
		else if (strcmp(argv[0], "mymv") == 0)    mv(argc, argv);
		else if (strcmp(argv[0], "myhelp") == 0)  help();
		else if (strcmp(argv[0], "cd") == 0)      cd(argc, argv);
		else if (strcmp(argv[0], "envir") == 0)   envir(argv[1]);
		else if (strcmp(argv[0], "type") == 0)    type(argv[1]);
		else if (strcmp(argv[0], "phist") == 0)   phist(hist, st, &c);
		else if (strcmp(argv[0], "exit") == 0)    break;
		else if (checkExtern(argv[0], command) > 0)       
	       	{	
			int ret = fork();
			if (ret > 0)
			{
				int status;
				wait(&status);
				addphist(hist, st, argv[0], status, &c);
			}
			else if (ret == 0) runExtern(argv, command);
			else               perror("Fork");
		}
		else                                      write(STDOUT, "Command not found.\n", 19);

		// Free up memory in argv
		for (int i = 0; i < argc; i++) free(argv[i]);
		free(argv);
	}

	// Exit message
	strcpy(command, "Farewell :(\n");
	write(STDOUT, command, strlen(command));

	return 0;
}


















