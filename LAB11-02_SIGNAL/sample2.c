#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>

void catchint(int);

int main(){
	int i, pid, status;

	static struct sigaction act;
	act.sa_handler = catchint;
	sigaction(SIGINT, &act, NULL);

	pid = fork();
	if(pid==0) {
		for(i=0;i<10;i++) {
			printf("child...\n");
			sleep(1);
		}
		exit(0);
	}else{
		sleep(5);
		kill(pid, SIGINT);
	}	
	wait(&status);
	if(WIFEXITED(status)) {
		printf("exits ... %d\n", WEXITSTATUS(status));
	}else if(WIFSIGNALED(status)) {
		printf("terminated ... %d\n", WTERMSIG(status));
	}
	exit(0);
}

void catchint(int signo) {
	printf("handled CATCHINT\n");
}