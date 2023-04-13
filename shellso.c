#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_CMD 1000
#define MAX_CMD_SIZE 1000
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_RESET "\x1b[0m"

void prompt()
{

    char cwd[MAX_CMD];
    if (getcwd(cwd, MAX_CMD) == NULL)
    {
        perror("getcwd() error()");
    }
    else
    {
        printf(ANSI_COLOR_RED "%s" ANSI_COLOR_RESET " $ ", cwd);
    }
}

char *readInput(int *background)
{

    char *buffer = (char *)calloc(sizeof(char), MAX_CMD);

    fgets(buffer, MAX_CMD, stdin);

    buffer[strlen(buffer) - 1] = '\0';

    if (buffer[strlen(buffer) - 1] == '&')
    {
        *background = 1;
        buffer[strlen(buffer) - 1] = '\0';
    }

    return buffer;
}

char ***splitInput(char *cmd, int *cmd_qtd)
{

    char ***commands = (char ***)calloc(sizeof(char **), MAX_CMD);
    char **tokens = (char **)calloc(sizeof(char *), MAX_CMD_SIZE);
    char *token;
    int index_cmd = 0, i;

    token = strtok(cmd, "|");

    for (i = 0; i < MAX_CMD; i++)
    {
        if (token == NULL)
        {
            break;
        }
        if (strlen(token) == 0)
        {
            i--;
        }
        tokens[i] = token;
        index_cmd++;
        token = strtok(NULL, "|");
    }
    *cmd_qtd = index_cmd;

    for (int index = 0; index < index_cmd; index++)
    {

        token = strtok(tokens[index], " ");
        commands[index] = (char **)calloc(sizeof(char *), MAX_CMD_SIZE);
        for (i = 0; token != NULL; i++)
        {
            commands[index][i] = token;
            token = strtok(NULL, " ");
        }
        commands[index][i] = NULL;
    }

    return commands;
}

void execute_commands(char ***commands, int num_cmd, int background)
{

    int fd[2];
    pid_t pid, pid_bg;
    int fd_in = STDIN_FILENO;

    pid_bg = fork();
    if (pid_bg == 0)
    {
        for (int i = 0; i < num_cmd; i++)
        {

            pipe(fd);

            if ((pid = fork()) == -1)
            {
                perror("fork() erro()");
                exit(1);
            }
            if (pid == 0)
            {
                dup2(fd_in, STDIN_FILENO);
                if (i < num_cmd - 1)
                {
                    dup2(fd[1], STDOUT_FILENO);
                }
                close(fd[0]);
                execvp(commands[i][0], commands[i]);
            }
            else
            {
                wait(NULL);
                close(fd[1]);
                fd_in = fd[0];
            }
        }
    }
    else
    {
        if (!background)
        {
            wait(NULL);
        }
    }
}

int main()
{
    char *cmd;
    char ***args;
    int cmd_qtd, background;
    pid_t p;

    system("clear");
    while (1)
    {

        prompt();
        background = 0;
        cmd = readInput(&background);

        if (strcmp(cmd, "\n") < 1)
        {
            free(cmd);
            continue;
        }

        args = splitInput(cmd, &cmd_qtd);

        execute_commands(args, cmd_qtd, background);
        for (int i = 0; i < cmd_qtd; i++)
        {
            free(args[i]);
        }
        free(args);
        free(cmd);
    }

    return 0;
}