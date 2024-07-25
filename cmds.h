#ifndef CMDS_H_
#define CMDS_H_

// COMMANDS
void getpwd ();
void echo   (const char *command, size_t argc);
void cpy    (size_t argc, char** argv);
void mv     (size_t argc, char** argv);
void help   ();

#endif
