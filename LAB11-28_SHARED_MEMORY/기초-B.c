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
	ushort* array;
};

int main(){
	int shmid, semid, i, n, *buf;
	key_t key1, key2;
	union semun arg;
	struct sembuf p_buf;

	// semaphore
	key1 = ftok("key", 1);
	semid = semget(key1, 1, 0600 | IPC_CREAT | IPC_EXCL);
	if(semid == -1) { 
		semid = semget(key1, 1, 0600); 
	}else{
		arg.val = 0;
		semctl(semid, 0, SETVAL, arg);
	}

	// shared memory	
	key2 = ftok("key", 2);
	shmid = shmget(key2, 10*sizeof(int), 0600 | IPC_CREAT);
	buf = (int*)shmat(shmid, 0, 0);

	for(i=0;i<10;i++){
		// semwait : until -1 is available its in BLOCKING STATE
		p_buf.sem_num = 0;
		p_buf.sem_op = -1;
		p_buf.sem_flg = 0;
		semop(semid, &p_buf, 1);

		printf("%d\n",*(buf+i));
	}
	shmctl(shmid, IPC_RMID, 0);
	semctl(semid, IPC_RMID, 0);

	exit(0);
}