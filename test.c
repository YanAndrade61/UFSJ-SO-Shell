#include <stdlib.h>
#include <stdio.h>
int main()
{

    FILE *fp = fopen(NULL, "w");

    if (fp != NULL)
        printf("1");
    else
        printf("0");

    return 0;
}