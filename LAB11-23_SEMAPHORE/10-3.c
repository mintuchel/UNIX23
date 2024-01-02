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

	// things we need for semctl
	union semun arg;
	ushort buf1[3] = {1,2,3}, buf2[3];	
	arg.array = buf;

	key = ftok("data", 3);
	// create semph that have 3 sems
	semid = semget(key, 3, 0600 | IPC_CREAT);
	
	printf("SETALL ... %d\n", semctl(semid, 0, SETALL, arg);

	arg.array = buf2;
	semctl(semid, 0, GETALL, arg);
	for(i=0;i<3;i++)
		printf("%d\n", arg.array[i]);
	return 0;
}