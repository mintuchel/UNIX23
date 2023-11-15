// 11-02 연습문제

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>

void catchint(int signo);

int main(){
    int i, j, num[10], sum = 0;
    // declare sigset to use
    sigset_t mask;

    // declare sigaction
    static struct sigaction act;
    act.sa_handler = catchint;
    sigaction(SIGINT, &act, NULL);

    // empty and add signal you want to block
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);

    for(i=0;i<5;i++){
        // set sigmask before certain task
        sigprocmask(SIG_SETMASK, &mask, NULL);
        scanf("%d", &num[i]);
        // remove signal after the task
        sigprocmask(SIG_UNBLOCK, &mask, NULL);

        sum+=num[i];

        for(j=0;j<=i;j++){
            printf("... %d\n", num[j]);
            sleep(1);
        }
    }
    exit(0);
}

void catchint(int signo){
    printf("DO NOT INTERRUPT ... \n");
}