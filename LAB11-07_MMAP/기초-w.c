#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

// writer 는 mmap된 파일을 읽기만 하면 된다. 
// 따라서 WRITE PERMISSION이 없어도 된다.
// 그래서 O_RDONLY로 열고 PROT_READ만 한다.

// 이미 reader에서 파일크기가세팅된 상황이므로 읽기만 하면 된다.

int main(){
	int fd, i;
	int* addr;
	
	fd = open("temp", O_RDONLY | O_CREAT, 0600);
	addr = mmap(NULL, 10*sizeof(int), PROT_READ, MAP_SHARED, fd ,0);

	sleep(5);
	for(i=0;i<5;i++){
		printf("%d\n", *(addr+i));
	}
	sleep(5);
	for(i=5;i<10;i++){
		printf("%d\n", *(addr+i));
	}

	exit(0);
}