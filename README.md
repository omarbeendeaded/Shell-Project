## Description
A simple shell program with 12 internal commands available:

```
1-  mycp     : copy file to another file
2-  mymv     : move file to another directory
3-  mypwd    : print current directory
4-  myecho   : print text after echo command to terminal
5-  myhelp   : print info about available commands
6-  exit     : terminates program
7-  cd       : changes current working directory
8-  type     : returns the type of the command
9-  envir    : print environment variables
10- phist    : print the last 10 processes with exit status
11- myfree   : print RAM info
12- uptime   : print up and idle time of the system
```
`Note:` Use `\ ` to indicate a space in your input
  
## Compilation Commands
```
$ gcc omarShell.c cmds.c -o shell
```


## Example Output
```
omar@Ubuntu22:~/Desktop/Task-2$ ./shell
/home/omar/Desktop/Task-2: Etfadal> ls
cmds.c  cmds.h  help.txt  omarShell.c  README.md  shell
/home/omar/Desktop/Task-2: Etfadal> 
/home/omar/Desktop/Task-2: Etfadal> 
/home/omar/Desktop/Task-2: Etfadal>  
/home/omar/Desktop/Task-2: Etfadal> type omar
Not supported
/home/omar/Desktop/Task-2: Etfadal> type cat
External
/home/omar/Desktop/Task-2: Etfadal> mycp cmds.h a.txt
/home/omar/Desktop/Task-2: Etfadal> ls
a.txt  cmds.c  cmds.h  help.txt  omarShell.c  README.md  shell
/home/omar/Desktop/Task-2: Etfadal> cat a.txt
#ifndef CMDS_H_
#define CMDS_H_

// COMMANDS
void getpwd      ();
void echo        (const char *command, int argc);
void cpy         (int argc, char** argv);
void mv          (int argc, char** argv);
void help        ();
void cd          (int argc, char** argv);
void runExtern   (char** argv, const char* path);
void envir       (char* var);
void type        (const char* cmd);

// HELPER
int    checkExtern  (const char* cmd, char* path);
int    countArgs    (const char *command);
int    checkExtern  (const char* cmd, char* path);
char** getArgs      (const char *command, int argc);

#endif
/home/omar/Desktop/Task-2: Etfadal> envir PATH
PATH=/home/omar/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/snap/bin
/home/omar/Desktop/Task-2: Etfadal> mkdir test
/home/omar/Desktop/Task-2: Etfadal> ls
a.txt  cmds.c  cmds.h  help.txt  omarShell.c  README.md  shell  test
/home/omar/Desktop/Task-2: Etfadal> cd test
/home/omar/Desktop/Task-2/test: Etfadal> exit
Farewell :(
omar@Ubuntu22:~/Desktop/Task-2$ 
```
