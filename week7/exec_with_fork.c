#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
    pid_t pid;

    // make child process
    pid = fork();

    // so here we have two processes in progress
    // one is parent and one is child

    switch(pid){
        // if fork error
        case -1:
            perror("fork failed");
            exit(1);
            break;
    
        // if fork success
        // and pid is child which pid value is 0
        case 0 :
            // change to "/bin/ls" code and execute ls command
            // so child code is totally changed
            execl("/bin/ls", "ls", "-l", (char*)0);
        
        // parent continues here
        default :
            // wait for child
            wait((int*)0);
            printf("ls completed\n");
            exit(0);
    }
}