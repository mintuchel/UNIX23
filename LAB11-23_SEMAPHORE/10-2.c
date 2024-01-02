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
	arg.val = 3;

	key = ftok("data", 1);
	// creat semph with one sem
	semid = semget(key, 1, 0600 | IPC_CREAT);
	// initialize #0 sem by arg
	semctl(semid, 0, SETVAL, arg);

	n = semctl(semid, 0, GETVAL, arg);
	printf("val: %d\n", n);
	n = semctl(semid, 0, GETPID, arg);
	printf("pid: %d\n", n);
	n = semctl(semid, 0, GETNCNT, arg);
	printf("ncnt: %d\n", n);
	n = semctl(semid, 0, GETZCNT, arg);
	printf("zcnt: %d\n", n);	
	return 0;
}