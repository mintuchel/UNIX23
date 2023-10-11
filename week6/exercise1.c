#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/errno.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

void do_child(){
	printf("pid=%ld, gid=%ld, sid=%ld\n", getpid(), getpgid(0), getsid(getpid()));
	exit(0);
}

int main(int argc,char** argv) {
	int num, i;
	pid_t pid;

	num = atoi(argv[1]);
	
	for(i=0;i<num;i++) {
		pid = fork();
		if(pid==0)
			do_child();
	}

	for(i=0;i<num;i++) {
		wait(0);
	}

	return 0;
}
