#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
    const char* av[] = {"test1","3","5","7", (char*)0};

    execvp("/.test1", av);
    exit(1);
}