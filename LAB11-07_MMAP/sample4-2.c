#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

int main(){
	int fd, i;
	char* addr;
	
	fd = open("data6", O_RDWR);
	printf("%d\n", fd);
	
	addr = mmap(NULL, 10, PROT_WRITE, MAP_SHARED, fd, 0);
	printf("%p\n", addr);

	for(i=0;i<5;i++)
		*(addr+i) = *(addr+i) + 1;

	for(i=0;i<5;i++)
		printf("%c ...\n", *(addr+i));
	
	exit(0);
}