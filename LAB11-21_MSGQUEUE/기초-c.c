#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct q_entry{
 long mtype;
 int data;
};

int main(int argc, char** argv){
 	int i, qid, in, id;
 	key_t key;
 	struct q_entry msg;

 	id = atoi(argv[1]);

 	key = ftok("key", 3);
	// open msgqueue
	// if doesnt exists, create and return
	// if exists, just return
	qid = msgget(key, 0600);

	for(i=0;i<5;i++){
		scanf("%d", &in);
		// set msg mtype
		msg.mtype = id;
		msg.data = in;
		msgsnd(qid, &msg, sizeof(int), 0);
		// server sends msg
		// get msg that is id+3 type
		// msgrcv is BLOCKED until there is a msg to rcv
		msgrcv(qid, &msg, sizeof(int), id+3, 0);
		printf("%d\n", msg.data);
	}
	return 0;
}