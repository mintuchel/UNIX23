#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/wait.h>

int main(){
	int i, status;
	pid_t pid;

	pid = fork();
	if(pid==0) {
		for(i=0;i<100;i++) {
			printf("child...\n");
			sleep(1);
		}
		exit(0);
	}else{
		sleep(5);
		kill(pid, SIGINT);
	}
	wait(&status);
	if(WIFEXITED(status))
		printf("exits... %d\n", WEXITSTATUS(status));
	else if(WIFSIGNALED(status))
		printf("terminated... signum:%d\n", WTERMSIG(status));
	exit(0);
}