#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <fcntl.h>

struct databuf{
	int d_nread;
	char d_buf[512];
}

int main(){
	char* buf1;
	int i, n, shmid1, shmid2, shmid3, *buf2;
	key_t key1, key2, key3;
	struct databuf *buf3;

	key1 = ftok("key", 1);
	key2 = ftok("key", 2);
	key3 = ftok("key", 3);
	
	// get shared memory descriptor by shmget	
	shmid = shmget(key1, 512, 0600 | IPC_CREAT);
	shmid = shmget(key2, 10*sizeof(int), 0600 | IPC_CREAT);
	shmid = shmget(key3, 5*sizeof(struct databuf), 0600 | IPC_CREAT);

	// attach shared memory to process's DATA AREA
	buf1 = (char*)shmat(shmid1, 0, 0);
	buf2 = (int*)shmat(shmid2, 0, 0);
	buf3 = (struct databuf*)shmat(shmid3, 0, 0);

	// 3 different ways to read shared memory
	n = read(0, buf1, 512);
	write(1, buf1, 512);

	for(i=0;i<10;i++) scanf("%d", buf2+i);
	for(i=0;i<10;i++) printf("%d\n", *(buf2+i));

	for(i=0;i<5;i++){
		n = read(0, (buf3+i)->d_buf, 512);
		(buf3+i)->d_nread = n;
	}
	for(i=0;i<5;i++){
		printf("%d ... %s", (buf3+i)->d_nread, (buf3+i)->d_buf);
	}

	// removed shared memory by using shmctl
	shmctl(shmid1, IPC_RMID, 0);
	shmctl(shmid2, IPC_RMID, 0);
	shmctl(shmid3, IPC_RMID, 0);
	exit(0);
}