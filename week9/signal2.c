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

void sigfunc();

// 인자로 pid_t 배열 받음
// 다른 자식들꺼까지 모두 알고 있음
void do_child(int i, int* cid) {
	
	int j;
	pid_t pid;
	
	// just for sigaction setting
	static struct sigaction act;
	act.sa_handler = sigfunc;
	sigaction(SIGUSR1, &act, NULL);
	
	// wait for signal
	if(i<4)	pause();

	pid = getpid();
	for(j=0;j<5;j++){
		printf("child %d ... \n", pid);
		sleep(1);
	}
	
	// send SIGUSR1 signal to next child
	if(i>0) kill(cid[i-1], SIGUSR1);

	exit(0);
}

int main(){
	int i, status;
	pid_t pid[5];

	for(i=0;i<5;i++){
		pid[i] = fork();
		if(pid[i]==0){
			do_child(i, pid);
		}
	}

	// wait for 5 childs
	for(i=0;i<5;i++){
		wait(&status);
	}

	exit(0);
}

void sigfunc(){
	printf("child got signal\n");
}
