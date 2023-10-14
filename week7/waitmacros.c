#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
    pid_t pid;

    // variables to get child's exit status infos
    int status, exit_status;

    pid = fork();

    if(pid<0){
        perror("fork failed");
        exit(1);
    }

    if(pid==0){
        sleep(4);
        exit(5);
    }

    // parent gets child's pid if child is finished
    // and child's exit infos are saved in the given status variable
    // if wait have error, wait returns -1
    if( (pid=wait(&status))==-1){
        perror("wait failed");
        exit(2);
    }

    // By checking status variable,
    // parent can know if child exit is successful
    // WIFEXITED checks if the last 8 bits are all 0 -> exit successful
    if(WIFEXITED(status)){

        // gets exit number by using status variable
        exit_status = WEXITSTATUS(status);
        
        printf("Exit status from %d was %d\n", pid, exit_status);
    }
    
    exit(0);
}