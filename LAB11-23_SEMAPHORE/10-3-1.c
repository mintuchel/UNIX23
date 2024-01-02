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

int main(int argc, char** argv){
	int semid, n, k;
	key_t key;
	union semun arg;
	struct sembuf p_buf;

	k = atoi(argv[1]);
	
	key = ftok("key", 6);
	semid = semget(key, 1, 0600 | IPC_CREAT | IPC_EXCL);
	if(semid==-1){
		semid = semget(key, 1, 0600);
	}else{
		arg.val = 1;
		semctl(semid, 0, SETVAL, arg);
	}

	printf("sem val : %d\n", semctl(semid, 0, GETVAL, arg));

	p_buf.sem_num = 0;
	p_buf.sem_op = -k;
	p_buf.sem_flg = 0;

	semop(semid, &p_buf, 1);

	printf("process %d in critical section...\n", getpid());
	sleep(5);
	printf("process %d leaving critical section...\n", getpid());
	
	p_buf.sem_num = 0;
	p_buf.sem_op = k*2;
	p_buf.sem_flg = 0;
	semop(semid, &p_buf, 1);

	printf("sem val : %d\n", semctl(semid, 0, GETVAL, arg));
	return 0;
}