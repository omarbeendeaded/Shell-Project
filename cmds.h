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
