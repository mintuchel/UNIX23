// LAB10-31 기초.c

#include<signal.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>

void catchusr();

void do_child(int N){
	int i;
	
	// just for setting sigaction
	static struct sigaction act;
	act.sa_handler =  catchusr;
	sigaction(SIGUSR1, &act, NULL);

	printf("%d-th child is created...\n", N);
	
	// wait for a signal 
	pause();

	// signal 받으면 자신의  pid 3번 출력
	for(i=0;i<3;i++) {
		printf("pid: %d\n", getpid());
	}

	exit(1);
}

int main(){
	int i, cid, status;
	pid_t pid[3];

	for(i=0;i<3;i++){
		pid[i] = fork();
		if(pid[i]==0){
			do_child(i);
		}
	}
	
	for(i=0;i<3;i++){
		sleep(1);
		// send SIGUSR1 signal to pid[i] process
		kill(pid[i], SIGUSR1);
	}

	// at this time, all the child processes are exited
	for(i=0;i<3;i++){
		// so catch all 3 child process's status
		cid = wait(&status);
		printf("child id=%d, exit status=%d\n", cid, WEXITSTATUS(status));
	}

	exit(0);
}

void catchusr() {
	// codes
	printf("catchusr called\n");
}
