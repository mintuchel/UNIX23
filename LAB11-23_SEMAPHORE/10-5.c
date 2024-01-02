#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <fcntl.h>

union semun{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
};

int main(){
	int semid, i;
	key_t key;
	union semun arg;
	
	ushort buf1[3] ={1,2,3};
	ushort buf2[3] = {0};
	// used in semop
	struct sembuf p_buf[2] = {{0,-1,0}, {1,-1,0}};

	key = ftok("key", 3);
	semid = semget(key, 3, 0600 | IPC_CREAT);

	arg.array = buf1;
	printf("SETALL ... %d\n", semctl(semid, 0, SETALL, arg));

	arg.array = buf2;
	printf("GETALL ... %d\n", semctl(semid, 0, GETALL, arg));
	
	for(i=0;i<3;i++) {
		printf("%d\n", arg.array[i]);
	}

	semop(semid, p_buf, 2);

	printf("GETALL ... %d\n", semctl(semid, 0, GETALL, arg));
	for(i=0;i<3;i++) {
		printf("%d\n", arg.array[i]);
	}
	sleep(5);

	return 0;
}