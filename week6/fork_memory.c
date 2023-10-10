#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

// this is an inapproporiate way to do fork
// for better way check fork_exit.c
// code just for understanding fork

int main(int argc, char** argv){
    int i, N;
    pid_t pid;

    N = atoi(argv[1]);

    printf("pid=%ld ppid=%ld\n", getpid(), getppid());

    // child copies not only the code but the whole memory that parent was using
    // so child runs the while loop too

    // ex) if parent i was 3 and child is forked, the child i is 3 too.
    for(i=0;i<N;i++){    
        pid = fork();
        if(pid==0) {
            printf("pid=%ld ppid=%ld", getpid(), getppid());
        }
    }

    for(i=0;i<N;i++){
        if(pid>0){
            wait(0);
        }
    }

    return 0;
}