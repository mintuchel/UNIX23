#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
	char* addr;
	int fd1, fd2, i, n, len = 0;
	pid_t* pid;
	
	fd1 = open("data", O_RDWR|O_CREAT, 0600);
	fd2 = open("temp", O_RDWR|O_CREAT, 0600);
	
	pid = mmap(NULL, sizeof(pid_t), PROT_WRITE, MAP_SHARED, fd1, 0);
	addr = mmap(NULL, 512, PROT_READ, MAP_SHARDED, fd2, 0);
	
	ftruncate(pid, sizeof(pid_t));
	*pid = getpid();

	for(i=0;i<3;i++) {
		// wait for signal from reader
		pause();
		
		write(1, addr, 512);
		len = strlen(addr);
		write(1, "-------\n", 8);
		if(len > 512) 
			break;
	}
	exit(0);
}

void catchsig(int signo) {

}