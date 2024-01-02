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
	// used for semop command argument
	struct sembuf p_buf;
	
	key = ftok("data", 1);
	semid = semget(key, 1, 0600 | IPC_CREAT | IPC_EXCL);
	// if already exists
	if(semid==-1){
		semid = semget(key, 1, 0);
	}
	// if not, IPC_CREAT is succesful
	// initialize semph
	else{
		arg.val = 1;
		semctl(semid, 0, SETVAL, arg);
	}

	// setting sembuf arg before using it in semop
	p_buf.sem_num = 0;
	p_buf.sem_op = -1;
	p_buf.sem_flg = 0;
	semop(semid, &p_buf, 1);

	printf("process %d in critical section\n", getpid());
	sleep(5);
	printf("process %d leaving critical section\n", getpid());
	
	// setting sembuf arg before using it in semop
	p_buf.sem_num = 0;
	p_buf.sem_op = 1;
	p_buf.sem_flg = 0;
	semop(semid, &p_buf, 1);
	
	return 0;
}