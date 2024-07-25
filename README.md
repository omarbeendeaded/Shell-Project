## Description
A simple shell program with 6 different commands available:

```
1- cp   : copy file to another file
2- mv   : move file to another directory
3- pwd  : print current directory
4- echo : print text after echo command to terminal
5- help : print info about available commands
6- exit : terminates program
```
  
## Compilation Commands
```
$ gcc omarShell.c cmds.c -o shell
```


## Example Output
```
omar@Ubuntu22:~/Desktop/Task-2$ ./shell
Etfadal> help
Supported commands are:

1- cp   : copy file to another file
2- mv   : move file to another directory
3- pwd  : print current directory
4- echo : print text after echo command to terminal
5- help : print info about available commands
6- exit : terminates program
Etfadal> 
Etfadal> pwd
/home/omar/Desktop/Task-2
Etfadal> 
Etfadal> echo hello
hello
Etfadal> cp a.txt test
Etfadal> cp a.txt test
ERROR: File exists
Etfadal> cp -a a.txt test
Etfadal> 
Etfadal> mv test/a.txt /home/omar/Desktop/Task-2
ERROR: File exists
Etfadal> mv test/a.txt /home/omar/Desktop/Task-2 b.txt
Etfadal> 
Etfadal> 
Etfadal> exit
Farewell :(
omar@Ubuntu22:~/Desktop/Task-2$ 
```
