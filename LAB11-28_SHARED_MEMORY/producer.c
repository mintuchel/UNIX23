#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

union semun{
	int val;
	struct semid_ds *buf;
	unsigned short * array;
};

struct buffer {
	int f;
	int r;
	char msg[10][512];
};

int main(){
	char temp[512];

	// use 3 semaphores
	ushort buf[3] = {1, 0, 10};
	
    int i, fd, n;
	int semid, shmid;

	key_t key1, key2;
	union semun arg; // initialize sem set
	struct buffer* msg_buf;
	struct sembuf p_buf; // for semop

	// semaphore 생성 및 초기값 설정
	key1 = ftok("key", 5);
        // get semaphore set that have 3 semaphores
	semid = semget(key1, 3, 0600 | IPC_CREAT | IPC_EXCL);
	if(semid == -1) {
		semid = semget(key1, 3, 0600);
	}else{
		// initialize semaphore set
		arg.array = buf;
		semctl(semid, 0, SETALL, arg);
	}
	
	// shared memory 생성 및 부착
	key2 = ftok("key", 2);
	shmid = shmget(key2, sizeof(struct buffer), 0600 | IPC_CREAT);
	msg_buf = (struct buffer*)shmat(shmid, 0, 0);
	msg_buf->f = msg_buf ->r = 0;
	
	do{
		// produce
		scanf("%s", temp);
		
		// semaphore 이용 동기화
		// semwait(e)
		p_buf.sem_num = 2;
		p_buf.sem_op = -1;
		p_buf.sem_flg = 0;
		semop(semid, &p_buf, 1);
	
		// semwait(s)
		p_buf.sem_num = 0;
		p_buf.sem_op = -1;
		p_buf.sem_flg = 0;
		semop(semid, &p_buf, 1);
	
		// temp를 shared memory에 복사
		strcpy(msg_buf->msg[msg_buf->r], temp);
		printf("idx=%d : msg=%s\n", msg_buf->r, msg_buf->msg[msg_buf->r]);
		
		// shared memory의 index 조정
		msg_buf->r = (msg_buf->r + 1) % 10;
		
		// semaphore 이용 동기화
		// semsignal(s)
		p_buf.sem_num = 0;
		p_buf.sem_op = 1;
		p_buf.sem_flg = 0;
		semop(semid, &p_buf, 1);

		// semsignal(n)
		p_buf.sem_num = 1;
		p_buf.sem_op = 1;
		p_buf.sem_flg = 0;
		semop(semid, &p_buf, 1);
	}while(strcmp(temp,"quit"));
	
	exit(0);
}