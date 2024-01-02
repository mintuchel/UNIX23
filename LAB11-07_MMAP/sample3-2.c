#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
	int fd, i, *addr;
	pid_t pid;
	
	fd = open("data5", O_RDWR | O_CREAT, 0600);
	if(fork()==0) {
		sleep(5);
		printf("child...\n");
		addr = mmap(NULL, 50, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		printf("%d\n", *(addr+4));
		exit(0);
	}else{
		printf("parent...\n");
		addr = mmap(NULL, 50, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		ftruncate(fd, sizeof(int)*5);
		
		for(i=0;i<5;i++)
			*(addr+i) = i;
	}
	wait(0);
	exit(0);
}