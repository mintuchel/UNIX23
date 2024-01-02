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
	ushort *array;
};

int main(){
	int shmid, semid, i, n, *buf;
 	key_t key1, key2;
 	union semun arg;
 	struct sembuf p_buf; // used for semop
	
	// semaphore
	key1 = ftok("key", 1);
	semid = semget(key1, 1, 0600 | IPC_CREAT | IPC_EXCL);
	if(semid == -1) { 
		semid = semget(key1, 1, 0600); 
	}
	else{
		arg.val = 0;
		semctl(semid, 0, SETVAL, arg);
	}

	// shared memory
	key2 = ftok("key", 2);
	shmid = shmget(key2, 10*sizeof(int), 0600 | IPC_CREAT);
	// attach shared memory to process
	buf = (int*)shmat(shmid, 0, 0);

	for(i=0;i<10;i++) {
		// write on shared memory
		scanf("%d", buf+i);

		// semsignal
		p_buf.sem_num = 0;
		p_buf.sem_op = 1;
		p_buf.sem_flg = 0;
		semop(semid, &p_buf, 1);		
	}
	exit(0);
}