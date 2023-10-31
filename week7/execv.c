#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

// execute with vector
// gives arguments as char* vector

int main(){
    char* const av[] = {"a.out", "3", (char*)0};

    printf("executing a.out\n");

    execv("./a.out",av);

    printf("execv failed to run a.out");
    exit(1);
}