#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
    mkdir("A",0700);
    chdir("A");
    
    mkdir("B", 0700);
    chdir("B");

    mkdir("C", 0700);

    // go back to starting directory
    chdir("../../..");

    symlink("A/B/C","T");

    return 0;
}