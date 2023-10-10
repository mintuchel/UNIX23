#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void do_child(){
    printf("pid=%ld, gid=%ld, sid=%ld\n", getpid(), getpgid(0), getsid(getpid()));
    exit(0);
}

int main(int argc, char** argv){
    int i, N;
    pid_t pid;

    N = atoi(argv[1]);
    printf("pid=%ld, gid=%ld, sid=%ld\n", getpid(), getgrid(), getsid());

    for(i=0;i<N;++){
        pid=fork();
        if(pid==0){
            do_child();
        }
    }

    for(i=0;i<N;i++){
        wait(0);
    }

    return 0;
}