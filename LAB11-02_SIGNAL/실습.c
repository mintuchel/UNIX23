#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/errno.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <signal.h>

void catchusr(int);

void do_child(void)
{
	int i, pid, gid;

	static struct sigaction act;	
	act.sa_handler = catchusr;
	sigaction(SIGUSR1, &act, NULL);

	//pid = getpid();
	//setpgid(pid, pid);

	pid = fork();

	// wait until signal comes
	// 2 childs are stopped here. Does not process until they get the signal
	pause();

	// this is done by both childs
	gid = getpgid(0);

	for(i=0;i<3;i++) {
		printf("group id=%d ... process id=%d\n", gid, getpid());
		sleep(1);
	}

	// only for child's child
	if(pid>0){
		pid = wait(0);
		printf("level 2 : process %d safely terminated...\n", pid);
	}
	exit(0);
}

int main(void){
	int i;
	pid_t pid[2];

	for(i=0;i<2;i++) {
		pid[i] = fork();
		if(pid[i]==0){
			do_child();
		}
	}

	printf("parent process id : %d group id : %d\n\n", getpid(), getpgid(0));
	
	// every 4 childs are in pause mode
	// waiting for SIGNAL
	sleep(2);
	//kill(pid[0], SIGUSR1);
	kill(0,SIGUSR1);
	sleep(5);
	kill(0,SIGUSR1);
	//kill(pid[1], SIGUSR1);

	for(i=0;i<2;i++) {
		pid[i] = wait(0);
		printf("level 1 : process %d safely terminated...\n", pid[i]);
	}
	exit(0);	
}

void catchusr(int signo){
	printf("got signal\n");
}