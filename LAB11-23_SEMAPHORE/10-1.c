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
	int semid, n;
	key_t key;
	union semun arg;
	struct semid_ds temp;

	key = ftok("data", 1);
	printf("%d\n", semid = semget(key, 3, 0600 | IPC_CREAT));
	
	arg.buf = &temp;
	printf("%d\n", semctl(semid, 0, IPC_STAT, arg));
	printf("%d %d\n", arg.buf->sem_perm.uid, getuid());

	semctl(semid, 0, IPC_RMID, 0);
	return 0;
}