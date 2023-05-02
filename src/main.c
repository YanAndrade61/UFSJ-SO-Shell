#include"shellso.h"

int main(int argc, char *argv[]){

    int fd;

    if(argc > 2){
        printf("Too many arguments\nExecute: ./shellso cmd_file <optional>");
        exit(1);
    }

    //If a file is passed as argument, use file descriptor of the file as default read
    if(argc == 2){
        fd = get_fd(argv[1],"r");
        if(fd == -1){
            perror("get_fd() error()");
            exit(1);
        }
        dup2(fd,STDIN_FILENO);
    }

    shell();

    return 0;
}