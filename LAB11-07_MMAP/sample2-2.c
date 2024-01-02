#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(){
	int fd, i;
	char* addr;
	pid_t pid;

	fd = open("data", O_RDWR);
	
	pid = fork();
	if(pid==0) {
		addr = mmap(NULL, 50, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		printf("child : %s\n", addr);
		exit(0);
	}else{
		sleep(2);
		addr = mmap(NULL, 50, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
		for(i=0;i<5;i++)
			*(addr+i) = *(addr+i) + 1;
		printf("parent : %s\n", addr);
	}

	wait(0);

	addr = mmap(NULL, 50, PROT_READ, MAP_SHARED, fd, 0);
	printf("original : %s\n", addr);

	exit(0);
}