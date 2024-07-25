#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define STDOUT 1


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
void echo(const char *text, size_t argc) 
{
	if (argc > 1)
	{
		write(STDOUT, text, strlen(text));
		write(STDOUT, "\n", 1);
	}
}



// --------- CMD: cp ----------
void cpy(size_t argc, char** argv)
{

	char append = 0;
	char wrongOption = 0;
	size_t opt;

	// Get options
	while ((opt = getopt(argc, argv, "a:")) != -1)
	{
		switch (opt)
		{
			case 'a':
				append = 1;
				break;
			default:
				wrongOption = 1;
				break;		
		}

	}
	
	// Check correct usage
	if ((!append && (argc !=  3 && argc !=  4)) || (append && (argc != 4 && argc != 5 )) || (append && strcmp(argv[1],"-a") != 0) || wrongOption)
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
	struct stat s;
	int trgIsDir = 0;
	if (!stat(trg, &s) &&  S_ISDIR(s.st_mode)) trgIsDir = 1;


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
		for (size_t i = 0; i < strlen(src); i++) if (src[i] == '/') fileStart = i;
		
		strcat(trg, "/");
		strcat(trg, argv[1 + append] + fileStart + 1);
	}

	// Open src file 
	int fd1 = open(src, O_RDONLY);
	if (fd1 < 0)
       	{
		write(STDOUT, "ERROR: ", 7);
		write(STDOUT, strerror(errno), strlen(strerror(errno)));
		write(STDOUT, "\n", 1);
	       	return;
	}


	// Create new target file
	int fd2 = open(trg, O_WRONLY|O_CREAT|(O_EXCL * !append)|(O_APPEND * append), S_IRWXU);
	if (fd2 < 0)
	{
		write(STDOUT, "ERROR: ", 7);
		write(STDOUT, strerror(errno), strlen(strerror(errno)));
		write(STDOUT, "\n", 1);
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
void mv(size_t argc, char** argv)
{
	char force = 0;
	char wrongOption = 0;
	size_t opt;

	// Get options
	while ((opt = getopt(argc, argv, "f:")) != -1)
	{
		switch (opt)
		{
			case 'f':
				force = 1;
				break;
			default:
				wrongOption = 1;
				break;		
		}

	}
	
	// Check correct usage
	if ((!force && (argc !=  3 && argc !=  4)) || (force && (argc != 4 && argc != 5 )) || (force && strcmp(argv[1],"-f") != 0) || wrongOption)
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
	struct stat s;
	int trgIsDir = 0;
	if (!stat(trg, &s) &&  S_ISDIR(s.st_mode)) trgIsDir = 1;


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
		for (size_t i = 0; i < strlen(src); i++) if (src[i] == '/') fileStart = i;
		
		strcat(trg, "/");
		strcat(trg, argv[1 + force] + fileStart + 1);
	}

	// Open src file 
	int fd1 = open(src, O_RDONLY);
	if (fd1 < 0)
       	{
		write(STDOUT, "ERROR: ", 7);
		write(STDOUT, strerror(errno), strlen(strerror(errno)));
		write(STDOUT, "\n", 1);
	       	return;
	}


	// Create new target file
	int fd2 = open(trg, O_WRONLY|O_CREAT|(O_EXCL * !force), S_IRWXU);
	if (fd2 < 0)
	{
		write(STDOUT, "ERROR: ", 7);
		write(STDOUT, strerror(errno), strlen(strerror(errno)));
		write(STDOUT, "\n", 1);
		return;
	}


	// Copy contents
	char buff[200];
	size_t count;
	while ((count = read(fd1, buff, 200)) != 0) write(fd2, buff, count);

	// Remove original file
	unlink(src);

	// CLose files
	close(fd1);
	close(fd2);
}



// --------- CMD: help ----------
void help()
{
	// Open txt file containing info
	int fd = open("help.txt", O_RDONLY);
	if (fd < 0)
       	{
		write(STDOUT, "ERROR: ", 7);
		write(STDOUT, strerror(errno), strlen(strerror(errno)));
		write(STDOUT, "\n", 1);
	       	return;
	}
	
	// Print to STDOUT
	char buff[200];
	size_t count = 0;
	while ((count = read(fd, buff, 200)) != 0) write(STDOUT, buff, count);

	
	// Close file
	close(fd);
}





















