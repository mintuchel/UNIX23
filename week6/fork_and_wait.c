#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
    pid_t pid;
    printf("pid=%ld ppid=%ld", getpid(), getppid());

    pid = fork();

    // child process
    if(pid==0){
        printf("pid=%ld ppid=%ld\n", getpid(), getppid());
        return 0;
    }
    // the parent waits until the child process end
    // and get the call that child process ended
    wait(0);
    
    return 0;
}