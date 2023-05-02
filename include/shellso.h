#ifndef __SHELLSO_H__
#define __SHELLSO_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_CMD 1000
#define MAX_CMD_SIZE 1000
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

int shell();
void prompt();
int get_fd(char *path, char *mode);
int readInput(char* cmd_line, int *background);
char ***processInput(char *cmd, int *cmd_qtd, int fd_redirect[]);
void execute_commands(char ***commands, int num_cmd, int fd_redirect[], int background);
int builtinCommands(char ***commands);


#endif