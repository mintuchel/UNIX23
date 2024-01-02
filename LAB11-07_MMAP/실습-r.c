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

	// file open	
	fd1 = open("data", O_RDWR | O_CREAT, 0600);
	fd2 = open("temp", O_RDWR | O_CREAT, 0600);

	pid = mmap(NULL, sizeof(pid_t), PROT_READ, MAP_SHARED, fd1, 0);
	addr = mmap(NULL, 512, PROT_WRITE, MAP_SHARDED, fd2, 0);

	// since we created new file, we have to increase file size
	// to avoid bus error
	ftruncate(fd1, 512);
	ftruncate(fd2, 512);
	
	// in order to send signal to the writer,
	// we need to know the writer process's pid
	while(*pid==0);	
	printf("writer id = %ld\n", *pid);

	for(i=0;i<3;i++) {
		// read from console input
		// and save it in mmaped memory
		read(0, addr+len, 512-len);
		// *****; msync?
		if(len>=512)
			break;
		// after saved,
		// send signal to writer
		kill(SIGUSR1, pid);	
	}

	exit(0);
}