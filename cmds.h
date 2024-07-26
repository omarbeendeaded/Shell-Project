#ifndef CMDS_H_
#define CMDS_H_

// COMMANDS
void getpwd ();
void echo   (const char *command, int argc);
void cpy    (int argc, char** argv);
void mv     (int argc, char** argv);
void help   ();

#endif
