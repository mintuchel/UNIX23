#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
	int i, in, fd, turn, n;
	pid_t pid;
	struct flock lock;

	fd = open("data2", O_RDWR | O_CREAT, 0600);
	
	// 동기화를 위한 locking 연산
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = sizeof(int);
	fcntl(fd, F_SETLKW, &lock);

	pid = getpid();
	for(i=0;i<5;i++) {
		sleep(1);
		printf("%ld\n", pid);
	}

	// 동기화를 위한 locking 연산
	lock.l_type = F_UNLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = sizeof(int);
	fcntl(fd, F_SETLK, &lock);
	
	exit(0);
}