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
	unsigned short* array;
};

int main(){
	int semid, n, k;
	key_t key;
	union semun arg;
	struct sembuf p_buf = {0, -1, 0};

	key = ftok("key", 7);
	semid = semget(key, 1, 0600 | IPC_CREAT | IPC_EXCL);
	if(semid==-1) {
		semid = semget(key, 1, 0600);
	}else{
		arg.val = 1;
		semctl(semid, 0, SETVAL, arg);
	}

	printf("process %d : %d --->", getpid(), semctl(semid, 0, GETVAL, 0));
	semop(semid, &p_buf, 1);
	printf("%d\n", semctl(semid, 0, GETVAL, 0));

	return 0;
}