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

/*Get file descriptor of an file*/
int get_fd(char *path, char *mode){
    FILE *fp = fopen(path, mode);

    if (fp == NULL)
        return -1;

    return fileno(fp);
}

/*Print the prompt (dir name)*/
void prompt()
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL){
        perror("getcwd() error()");
    }
    else{
        printf(ANSI_COLOR_RED "Dir: %s\n" ANSI_COLOR_RESET, cwd);
    }
}

/*Function to read input and generate an history*/
int readInput(char* cmd, int *background){

    char *buffer;
    
    buffer = readline("$ ");

    //Receive a crtl+d signal
    if(!buffer){
        return 2;
    }
    //Receive an empty string
    if(strlen(buffer) < 1){
        return 1;
    }

    //Check if the command will run in background
    if (buffer[strlen(buffer) - 1] == '&'){
        *background = 1;
        buffer[strlen(buffer) - 1] = '\0';
    }
    
    add_history(buffer);
    strcpy(cmd,buffer);
    free(buffer);
    return 0;
}

/*Process the cmd line, separating each command and their args*/
char ***processInput(char *cmd, int *cmd_qtd, int fd_redirect[])
{

    char ***commands = (char ***)calloc(sizeof(char **), MAX_CMD);
    char **tokens = (char **)calloc(sizeof(char *), MAX_CMD_SIZE);
    char *token;
    int i;

    //Get each commands in line separated by pipe
    token = strtok(cmd, "|");
    for (i = 0; i < MAX_CMD && token != NULL; i++)
    {
        if (strlen(token) == 0){
            i--;
        }
        else{
            tokens[i] = token;
        }       
        token = strtok(NULL, "|");
    }
    *cmd_qtd = i;
    
    for (int index = 0; index < *cmd_qtd; index++){

        //Get each command args separated by space
        token = strtok(tokens[index], " ");
        commands[index] = (char **)calloc(sizeof(char *), MAX_CMD_SIZE);
        for (i = 0; i < MAX_CMD_SIZE && token != NULL; i++){
            commands[index][i] = token;
            token = strtok(NULL, " ");
        }
        commands[index][i] = NULL;

        //Check if that is an out redirect in last command
        if (i >= 2 && index == *cmd_qtd - 1 && strcmp(commands[index][i - 2], "=>") == 0)
        {
            fd_redirect[1] = get_fd(commands[index][i - 1], "w");
            if (fd_redirect[1] == -1){
                perror("get_fd() error()");
            }
            commands[index][i - 2] = NULL;
            i -= 2;
        }

        //Check if that is an in redirect in first command
        if (i >= 2 && index == 0 && strcmp(commands[index][i - 2], "<=") == 0)
        {
            fd_redirect[0] = get_fd(commands[index][i - 1], "r");
            if (fd_redirect[0] == -1){
                perror("get_fd() error()");
            }
            commands[index][i - 2] = NULL;
        }
    }
    free(tokens);
    return commands;
}

void execute_commands(char ***commands, int num_cmd, int fd_redirect[], int background)
{

    int fd[2];
    int status;
    pid_t pid, pid_bg;
    int fd_in = STDIN_FILENO;
    
    //Redirect in of first command
    if (fd_redirect[0] != -1){
        fd_in = fd_redirect[0];
    }

    //Generate a fork to execute in background
    if ((pid_bg = fork()) == -1){
            perror("fork() erro()");
            exit(1);
    }
    if (pid_bg == 0){
        for (int i = 0; i < num_cmd; i++){

            pipe(fd);

            if ((pid = fork()) == -1){
                perror("fork() erro()");
                exit(1);
            }
            if (pid == 0){
                dup2(fd_in, STDIN_FILENO);
                if (i < num_cmd - 1){
                    dup2(fd[1], STDOUT_FILENO);
                }
                else if (fd_redirect[1] != -1){
                    dup2(fd_redirect[1], STDOUT_FILENO);
                }
                close(fd[0]);
                if(execvp(commands[i][0], commands[i]) < 0){
                    printf("\nError command %s",commands[i][0]);
                    exit(1);
                }
            }
            else
            {
                wait(NULL);
                close(fd[1]);
                fd_in = fd[0];
            }
        }
        exit(0);
    }
    else{
        if (background == 0){
            waitpid(pid_bg, &status, 0);
        }
    }
}

/*Execute builtin commands or return 0 if its not a builtin*/
int builtinCommands(char ***commands){

    if(strcmp(commands[0][0],"cd") == 0){
        chdir(commands[0][1]);
        return 1;
    }
    if(strcmp(commands[0][0],"fim") == 0){
        printf("See you later!\n");
        exit(0);
    }

    return 0;
}

int main()
{
    char cmd_line[MAX_CMD];
    char ***commands;
    int fd_redirect[2];
    int cmd_qtd, background, read_sig;
    pid_t p;

    system("clear");
    while (1)
    {
        prompt();
        
        //For default background and redirected are deactivate
        background = 0;
        memset(fd_redirect, -1, sizeof(int) * 2);

        read_sig = readInput(cmd_line, &background);
        //Receive a ctrl+d signal
        if(read_sig == 2){
            exit(0);
        }
        //Just an empty input
        if(read_sig == 1){
            continue;
        }

        commands = processInput(cmd_line, &cmd_qtd, fd_redirect);

        if(builtinCommands(commands)){
            continue;
        }

        execute_commands(commands, cmd_qtd, fd_redirect, background);

        for (int i = 0; i < cmd_qtd; i++)
        {
            free(commands[i]);
        }
        free(commands);
        // free(cmd);
    }

    return 0;
}