#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "cmds.c"

#define STDIN  0
#define STDOUT 1
#define STDERR 2

#define BUFF_SIZE 100

size_t countArgs(const char *command);
char** getArgs(const char *command, size_t argc);

int main(void)
{
	// Initialize shell
	char command[BUFF_SIZE];
	ssize_t readSize = 0;
	const char * shellMsg = "Etfadal> ";

	char **argv;
	size_t argc;

	while (1)
	{
		// Prompt & get input
		write(STDOUT, shellMsg, strlen(shellMsg));
		readSize = read(STDIN, command, BUFF_SIZE);
		command[readSize - 1] = '\0';

		// Parsing command
		argc = countArgs(command);
		if (argc > 0) argv = getArgs(command, argc);
		else continue;	

		// Check commands
		if (strcmp(argv[0], "pwd") == 0)       getpwd();
		else if (strcmp(argv[0], "echo") == 0) echo(command + 5, argc);
		else if (strcmp(argv[0], "cp") == 0)   cpy(argc, argv);
		else if (strcmp(argv[0], "mv") == 0)   mv(argc, argv);
		else if (strcmp(argv[0], "help") == 0) help();
		else if (strcmp(argv[0], "exit") == 0) break;
		else                                   write(STDOUT, "Command not found.\n", 19);


		// Free up memory in argv
		for (size_t i = 0; i < argc; i++) free(argv[i]);
		free(argv);
	}

	// Exit message
	strcpy(command, "Farewell :(\n");
	write(STDOUT, command, strlen(command));

	return 0;
}



// Returns number of space-separated arguments in string
size_t countArgs(const char *command)
{
	size_t count = 0;
	char cpy[BUFF_SIZE];
	char *token;

	strcpy(cpy, command);

	token = strtok(cpy, " ");
	while (token != NULL && token[0] != '\n')
	{
		count++;
		token = strtok(NULL, " ");
	}

	return count;
}




// Returns an array of space-separated arguements in string
char** getArgs(const char *command, size_t argc)
{

	char** args = malloc(argc * sizeof(char*));
	char cpy[BUFF_SIZE];
	char *token;

	strcpy(cpy, command);
	
	token = strtok(cpy, " ");
	for (size_t i = 0; i < argc; i++)
	{
		args[i] = strdup(token);
		token = strtok(NULL, " ");
	}

	
	return args;

}




















