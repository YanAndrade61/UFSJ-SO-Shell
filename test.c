#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{

    int c = 4;
    char buf[100];
    while(1){

        fgets(buf,100,stdin);
        if(buf[0] == (char)c)break;
    }
    return 0;
}