#include "shellso.h"

int get_fd(char *path, char *mode)
{
    FILE *fp = fopen(path, mode);

    if (fp == NULL)
        return -1;

    return fileno(fp);
}

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

void execute_commands(char ***commands, int num_cmd, int fd_redirect[], int background)
{

    int fd[2];
    pid_t pid, pid_bg;
    int fd_in = STDIN_FILENO;
    if (fd_redirect[0] != -1)
    {
        fd_in = fd_redirect[0];
    }
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
                else if (fd_redirect[1] != -1)
                {
                    dup2(fd_redirect[1], STDOUT_FILENO);
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
    int fd_redirect[2];
    int cmd_qtd, background;
    pid_t p;

    system("clear");
    while (1)
    {

        prompt();
        background = 0;
        memset(fd_redirect, -1, sizeof(int) * 2);

        cmd = readInput(&background);

        if (strcmp(cmd, "\n") < 1)
        {
            free(cmd);
            continue;
        }

        args = splitInput(cmd, &cmd_qtd, fd_redirect);

        execute_commands(args, cmd_qtd, fd_redirect, background);

        for (int i = 0; i < cmd_qtd; i++)
        {
            free(args[i]);
        }
        free(args);
        free(cmd);
    }

    return 0;
}