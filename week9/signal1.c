#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<ftw.h>
#include<time.h>
#include<signal.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/sem.h>
#include<sys/shm.h>

void catchint();

void do_child(int N){
	int i, pid;

	// just for sigaction setting
	// does not do anything
	static struct sigaction act;
	act.sa_handler =  catchint;
	sigaction(SIGUSR1, &act, NULL);

	printf("%d-th child is created...\n", N);
	
	// wait for an input signal 
	pause();

	// if signaled print pid 3 times
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

	// send each child process SIGUSR1 signal	
	for(i=0;i<3;i++){
		sleep(1);
		kill(pid[i], SIGUSR1);
	}

	for(i=0;i<3;i++){
		cid = wait(&status);
		printf("child id=%d, exit status=%d\n", cid, WEXITSTATUS(status));
	}

	exit(0);
}

void catchint() {
	// codes
	printf("catchint called\n");
}