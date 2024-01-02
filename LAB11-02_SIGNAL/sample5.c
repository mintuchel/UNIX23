#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>

void catchalarm(int);

int main(){
	int i, n;

	static struct sigaction act;
	act.sa_handler = catchalarm;
	sigaction(SIGALRM, &act, NULL);

	n = alarm(10);
	printf("알람설정: %d\n", n);

	for(i=0;i<10;i++) {
		printf("child...\n");
		if(i==1) {
			n = alarm(3);
			printf("알람재설정:%d\n", n);
		}
		sleep(1);
	}

	exit(0);
}

void catchalarm(int signo){
	printf("CATCHALRM signo=%d\n", signo);
	alarm(3);
}
