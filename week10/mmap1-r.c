// LAB11-07 기초-r.c

#include <sys/mann.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// reader는 scanf로 정수를 읽어서 mmap된 파일에 저장한다
// 따라서 temp도 WRTIE PERMISSION이 있게끔 열어준다
// mmap 시에 READ PERMISSION은 필수이므로 O_RDWR가 들어간다.

// mmap 파일에 쓸 것이므로 PROT_WRITE로 열어준다
int main(){
	int fd, i;
	int* addr;

	fd = open("temp", O_RDWR | O_CREAT, 0600);
	addr = mmap(NULL, 10*sizeof(int), PROT_WRITE, MAP_SHARED, fd, 0);
	
	// 방금 생성한 파일이므로 파일 크기를fd를 이용해  키워준다
	ftruncate(fd, sizeof(int)*10);

	printf("int scanf start\n");
	for(i=0;i<10;i++){
		scanf("%d", addr+i);
		// msync(addr, sizeof(int)*10, MS_SYNC);
	}

	exit(0);	
}
