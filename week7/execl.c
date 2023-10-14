#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(){
    printf("Before exec()\n");

    
    // replace the current process image with a new program
    // so only a.out process runs
    execl("./a.out", "a.out","3", (char*)0);
    
    // if the code comes here,
    // it means that execl failed so main is still running
    printf("execl failed to run a.out");
    exit(1);
}