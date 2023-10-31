#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void do_child(int num){
    int i;
    for(i=0;i<5;i++){
        printf("hi i am child #%d\n", num);
    }
    exit(num);
}

int main(){
    int i, status;
    pid_t pid[6];

    for(i=0;i<6;i++){
        pid[i] = fork();
        if(pid[i]==0){
            do_child();
        }
    }

    for(i=0;i<6;i++){
        waitpid(pid[i],&status, 0);
        if(WIFEXITED(status)){
            printf("\n", WEXITSTATUS(status));
        }
    }

    exit(0);
}