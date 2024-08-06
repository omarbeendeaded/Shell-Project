#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define STDOUT 1

#define BUFF_SIZE 200

extern char** environ;


///////// HELPER FUNCTIONS //////////

// ---------- Run External Commands ----------
void runExtern(char** argv, const char* path)
{
	execv(path, argv);
	perror("exec");
}

// Returns number of space-separated arguments in string
int countArgs(const char *command, const char* sep)
{
	int count = 0;
	char cpy[BUFF_SIZE];
	char *token;
	char flag = 0;

	strcpy(cpy, command);

	token = strtok(cpy, sep);
	while (token != NULL && token[0] != '\n')
	{
		flag = 0;
		count++;
		if (token[strlen(token) - 1] == '\\') count--, flag = 1; 
		token = strtok(NULL, sep);
	}

	if (flag) count++;

	return count;
}


// Returns an array of space-separated arguements in string
char** getArgs(const char *command, int argc, const char* sep)
{

	char** args = malloc((argc + 1) * sizeof(char*));
	char cpy[BUFF_SIZE];
	char *token;

	strcpy(cpy, command);
	
	token = strtok(cpy, sep);
	for (int i = 0; i < argc; i++)
	{
		args[i] = strdup(token);
		token = strtok(NULL, sep);

		// Handling spaces in file names
		if (args[i][strlen(args[i]) - 1] == '\\' && token != NULL)
		{	
			args[i][strlen(args[i]) - 1] = ' ';
			strcat(args[i], token);
			token = strtok(NULL, sep); 
		}
	}
	args[argc] = NULL;
	
	return args;

}



// Check external variable
int checkExtern(const char* cmd, char* path)
{
	char buff[BUFF_SIZE];
	int fd = open("/etc/environment", O_RDONLY);
	int readSize = read(fd, buff, BUFF_SIZE);
	buff[readSize - 2] = '\0';

	char *token;
	struct stat s;
	token = strtok(buff, "\"");

	while (1)
	{
		if ((token = strtok(NULL, ":")) == NULL ) break;
		
		strcpy(buff, token);
		strcat(buff, "/");
		strcat(buff, cmd);


		if (stat(buff, &s) == 0) 
		{
			strcpy(path, buff);
			return 1;
		}
	}
	
	return -1;
}


// Adds new commands to the list
void addphist(char hist[10][200], int st[10], char* cmd, int status, int* c)
{
	// Adds entry to end of the list
	if (*c < 10) 
	{
		strcpy(hist[*c], cmd);
		st[*c] = WEXITSTATUS(status);
		(*c)++;
	}
	// Replaces oldest entry with new one if list is full
	else if (*c == 10)
	{	
		// Shift entries one space
		for (int i = 0; i < 9; i++)
		{
			strcpy(hist[i], hist[i + 1]);
			st[i] = st[i + 1];
		}

		strcpy(hist[9], cmd);
		st[9] = WEXITSTATUS(status);
	}
}



/////////// COMMANDS ///////////


// ---------- CMD: pwd ----------
void getpwd()
{
	// Get directory
	char buff[200];
	getcwd(buff, 200);

	// Print directory
	write(STDOUT, buff, strlen(buff));
	write(STDOUT, "\n", 1);
}



// ---------- CMD: echo ---------
void echo(const char *text, int argc) 
{
	if (argc > 1)
	{
		write(STDOUT, text, strlen(text));
		write(STDOUT, "\n", 1);
	}
}



// --------- CMD: cp ----------
void cpy(int argc, char** argv)
{
	// Get options
	char append = 0;
	int opt;
	while ((opt = getopt(argc, argv, "a")) != -1)
	{
		switch (opt)
		{
			case 'a':
				append = 1;
				break;
			default:
				append = -1;
				break;
		}
	}
	
	// Check correct usage
	if ((argc !=  3 + append && argc !=  4 + append) || append == -1)
	{
		write(STDOUT, "Usage: cp [option] [source path] [target path] [file name]\n", 59);
		return;
	}
	
	
	// Set-up source and target paths
	char src[200];
	char trg[200];
	strcpy(src, argv[1 + append]);
	strcpy(trg, argv[2 + append]);

	
	// Check trg directory or file name
	struct stat s1, s2;
	int trgIsDir = 0;
	if (!stat(trg, &s1) &&  S_ISDIR(s1.st_mode)) trgIsDir = 1;


	// Changing file name if specified
	if ((!append && argc == 4) || (append && argc == 5))
	{
		if (trg[strlen(trg) - 1] != '/') strcat(trg, "/");
		strcat(trg, argv[3 + append]);
	}
	// Use directory specified
	else if (trgIsDir)
	{
		int fileStart = -1;
		for (int i = 0; i < strlen(src); i++) if (src[i] == '/') fileStart = i;
		
		strcat(trg, "/");
		strcat(trg, argv[1 + append] + fileStart + 1);
	}

	// Open src file 
	int fd1 = open(src, O_RDONLY);
	if (fd1 < 0)
       	{
		perror("Source file");
	       	return;
	}


	// Create new target file
	int fd2 = open(trg, O_WRONLY|O_CREAT|(O_EXCL * !append)|(O_APPEND * append), S_IRWXU);
	if (fd2 < 0)
	{
		perror("Target file");
		return;
	}

	// In case of same file
	fstat(fd1, &s1);
	fstat(fd2, &s2);
	if (s1.st_dev == s2.st_dev && s1.st_ino == s2.st_ino)
	{
		write(STDOUT, "ERROR: Cannot copy file into itself\n", 36);
		return;
	}
	

	// Copy contents
	char buff[200];
	size_t count;
	while ((count = read(fd1, buff, 200)) != 0) write(fd2, buff, count);

	// Close files
	close(fd1);
	close(fd2);
}



// --------- CMD: mv ----------
void mv(int argc, char** argv)
{
	// Get options
	int force = 0;
	if (argc > 1 && strcmp(argv[1], "-f") == 0) force = 1;
	
	// Check correct usage
	if (argc !=  3 + force && argc !=  4 + force)
	{
		write(STDOUT, "Usage: cp [option] [source path] [target path] [file name]\n", 59);
		return;
	}
	
	
	// Set-up source and target paths
	char src[200];
	char trg[200];
	strcpy(src, argv[1 + force]);
	strcpy(trg, argv[2 + force]);


	// Check trg directory or file name
	struct stat s1, s2;
	int trgIsDir = 0;
	if (!stat(trg, &s1) &&  S_ISDIR(s1.st_mode)) trgIsDir = 1;


	// Changing file name if specified
	if ((!force && argc == 4) || (force && argc == 5))
	{
		if (trg[strlen(trg) - 1] != '/') strcat(trg, "/");
		strcat(trg, argv[3 + force]);
	}
	// Use directory specified
	else if (trgIsDir)
	{
		int fileStart = -1;
		for (int i = 0; i < strlen(src); i++) if (src[i] == '/') fileStart = i;
		
		strcat(trg, "/");
		strcat(trg, argv[1 + force] + fileStart + 1);
	}

	// Open src file 
	int fd1 = open(src, O_RDONLY);
	if (fd1 < 0)
       	{
		perror("Source file");
	       	return;
	}


	// Create new target file
	int fd2 = open(trg, O_WRONLY|O_CREAT|(O_EXCL * !force), S_IRWXU);
	if (fd2 < 0)
	{
		perror("Target file");
		return;

	}


	// Copy contents
	char buff[200];
	size_t count;
	while ((count = read(fd1, buff, 200)) != 0) write(fd2, buff, count);

	// Remove original file
	fstat(fd1, &s1);
	fstat(fd2, &s2);
	if (!(s1.st_dev == s2.st_dev && s1.st_ino == s2.st_ino)) unlink(src);

	// CLose files
	close(fd1);
	close(fd2);
}



// --------- CMD: help ----------
void help()
{
	// Open txt file containing info
	int fd = open("helpers/help.txt", O_RDONLY);
	if (fd < 0)
       	{
		perror("Helper file");
	       	return;
	}
	
	// Print to STDOUT
	char buff[200];
	int count = 0;
	while ((count = read(fd, buff, 200)) != 0) write(STDOUT, buff, count);

	
	// Close file
	close(fd);
}



// ---------- CMD: cd ----------
void cd(int argc, char** argv)
{
	if (chdir(argv[1]) < 0) perror("cd");

}


// ---------- CMD: envir ----------
void envir(char* var)
{
	int i = 0;
	int loc = -1;

	if (var == NULL)
	{
		while (environ[i] != NULL)
		{
			write(STDOUT, environ[i], strlen(environ[i]));
			write(STDOUT, "\n", 1);
			i++;
		}
	}
	else
	{
		int found;
		while (environ[i] != NULL)
		{
			int j = 0;
			found = 1;
			while (environ[i][j] != '=' && j < strlen(var))
			{
				if (environ[i][j] != var[j])
				{
					found = 0;
					break;
				}
				j++;

			}
			
			if (found && environ[i][j] == '=' && j == strlen(var)) 
			{
				write(STDOUT, environ[i], strlen(environ[i]));
				write(STDOUT, "\n", 1);
				break;
			}
			
			i++;	
		}
		if (!found) write(STDOUT, "Not found.\n", 11);

	}
}

// ---------- CMD: type ----------
const char* COMMANDS[] = {"mycp", "mymv", "mypwd", "myecho", "myhelp", "exit", "cd", "type", "envir", "phist", "myfree", "uptime", NULL};
void type(const char* cmd)
{
	if (cmd == NULL) write(STDOUT, "Specify command.\n", 17);
	else
	{
		int i = 0;
		while (COMMANDS[i] != NULL)
		{
			if (strcmp(COMMANDS[i], cmd) == 0) 
			{
				write(STDOUT, "Internal\n", 9);
				return;
			}
			i++;
		}
		char buff[100];
		if (checkExtern(cmd, buff) == 1) write(STDOUT, "External\n", 9);
		else                             write(STDOUT, "Not supported\n", 14);
	}
}


// ---------- CMD: phist ----------
void phist(char hist[10][200], int st[10], int* c)
{
	for (int i = 0; i < *c; i++)
	{
		write(STDOUT, hist[i], strlen(hist[i]));
		write(STDOUT, " -- ", 4);
		printf("%d\n", st[i]);
	}
}


// --------- CMD: free --------
void myfree()
{
	int inds[100] = {0};
	int countInds = 0;
	int totalRead = 0;
	int readSize;
	int fd = open("/proc/meminfo", O_RDONLY);

	char buff[200];
	
	// Get offset of each line in the file
	while ((readSize = read(fd, buff, 200)) != 0)
	{
		for (int i = 0; i < readSize; i++)
		{
			if (buff[i] == '\n')
			{
				inds[countInds] = totalRead + i;
				countInds++;
			}
		}

		totalRead += readSize;
	}

	

	// Extract required data
	char nums[6][100];
	lseek(fd, 0, SEEK_SET);
	
	readSize = read(fd, buff, inds[0]);
	for (int i = 1; i < countInds; i++)
	{
		buff[readSize] = '\0';

		// Check for needed data
		int found = -1;
		if      (strncmp(buff, "MemTotal", 8) == 0)       found = 0;
		else if (strncmp(buff, "MemFree", 7) == 0)        found = 1;
		else if (strncmp(buff, "Buffers", 7) == 0)        found = 2;
		else if (strncmp(buff, "Cached", 6) == 0)         found = 3;
		else if (strncmp(buff, "SwapTotal", 9) == 0)      found = 4;
		else if (strncmp(buff, "SwapFree", 8) == 0)       found = 5;
		else if (strncmp(buff, "SReclaimable", 12) == 0)  found = 6;
		
		// Extract number
		if (found > -1)
		{
			int numStart = -1;
			for (int j = 0; j < readSize; j++)
			{
				if (numStart == -1 && buff[j] >= '0' && buff[j] <= '9') numStart = j;
				if (numStart > -1 && buff[j] == ' ') buff[j] = '\0';
			}
			strcpy(nums[found], buff + numStart);
		}
		memset(buff, 0, sizeof(buff));

		// Go to next line
		lseek(fd, inds[i - 1] + 1, SEEK_SET);
		readSize = read(fd, buff, inds[i] - inds[i - 1]);
	}


	// Printing output
	int MemTotal = atoi(nums[0]);
	int MemFree  = atoi(nums[1]);
	int MemUsed  = MemTotal - (MemFree + atoi(nums[2]) + atoi(nums[3]) + atoi(nums[6]));
	
	int SwapTotal = atoi(nums[4]);
	int SwapFree  = atoi(nums[5]);
	int SwapUsed  = SwapTotal - SwapFree;

	printf("Memory\n");
	printf(" Free: \t%d\n", MemFree);
	printf(" Used: \t%d\n", MemUsed);
	printf(" Total:\t%d\n", MemTotal);

	printf("\nSwap\n");
	printf(" Free: \t%d\n", SwapFree);
	printf(" Used: \t%d\n", SwapUsed);
	printf(" Total:\t%d\n", SwapTotal);
}


// --------- CMD: uptime ----------
void uptime()
{
	char buff[200];
	int fd = open("/proc/uptime", O_RDONLY);
	int readSize = read(fd, buff, 200);
	buff[readSize] = '\0';

	char* token;
	token = strtok(buff, " ");

	int up = atoi(token);
	int h = up / 3600;
	int m = (up - (h * 3600)) / 60;
	int s = up - (h * 3600) - (m * 60);
	printf("Uptime: %d:%02d:%02d\n", h, m, s);
	
	token = strtok(NULL, " ");

	up = atoi(token);
	h = up / 3600;
	m = (up - (h * 3600)) / 60;
	s = up - (h * 3600) - (m * 60);
	printf("Idle: %d:%02d:%02d\n", h, m, s);

}













