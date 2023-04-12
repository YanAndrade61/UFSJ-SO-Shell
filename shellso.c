#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#define MAX_BUFFER 10000 

void prompt(){

    char cwd[MAX_BUFFER];
    if(getcwd(cwd,MAX_BUFFER) == NULL){
        perror("getcwd() error()");
    }
    else{
        printf("%s $ ",cwd);
    }
}

char* readInput(){

    char* buffer = (char*) calloc(sizeof(char),MAX_BUFFER);

    fgets(buffer,MAX_BUFFER,stdin);

    buffer[strlen(buffer)-1] = '\0';

    return buffer;
}

// void splitInput(char* cmd,char **args){

//     char *token = NULL;
//     int i;

//     token = strtok(cmd," ");

//     for(i = 0; i < MAX_BUFFER && token != NULL; i++){
        
//         if(args[i] != NULL){
//             free(args[i]);
//         }
//         if(strlen(token) == 0){
//             i--;
//         }
//         args[i] = strdup(token);
//         token = strtok(NULL," ");
//     }

//     args[i] = NULL;

// }

char** splitInput(char* cmd){

    char **args = (char**) calloc(sizeof(char*), MAX_BUFFER);
    char *token;
    int i;

    token = strtok(cmd," ");
    
    for(i = 0; i < MAX_BUFFER; i++){
        if(token == NULL){
            break;
        }
        if(strlen(token) == 0){
            i--;
        }
        args[i] = token;
        token = strtok(NULL," ");
    }

    args[i] = NULL;
    
    return args;
}

int main(){
    char *cmd;
    char **args;
    pid_t p; 

    system("clear");
    while(1){
        
        prompt();
        cmd = readInput();

        if(strcmp(cmd,"\n") < 1){
            free(cmd);
            continue;
        }
        
        args = splitInput(cmd);

        if( (p = fork()) == -1){
            perror("fork() error()");
        }
        if(p == 0){
            execvp(args[0],args);
            exit(0);
        }
        else{
            wait(0);
        }
        free(cmd);
        free(args);
    }
        
    return 0;
}