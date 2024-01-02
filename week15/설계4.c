#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

struct q_entry{
    long mtype;
    int cnum;
    int s_id;
    char msg[512];
};

struct q_entry cmessage(int mtype, int cnum){
	struct q_entry msg;	
	msg.mtype=mtype; // 999
	msg.cnum=cnum; // 현재 client 수
	msg.s_id=0;
	strcpy(msg.msg, "");
	return msg;
}

struct q_entry nmessage(int mtype, int s_id, char *str){
	struct q_entry msg;
	msg.mtype=mtype; // 모든 메시지의 타입이 다름 
	msg.cnum=0;
	msg.s_id=s_id;
	strcpy(msg.msg, str);
	return msg;
}

//struct q_entry cmessage(int mtype, int cnum);
//struct q_entry nmessage(int mtype, int s_id, char *str);

void do_sender(int qid, int id){
	char temp[512];
    int i, cnum, index=1;
    struct q_entry msg1;

	// (e) message 보내기 전 준비
	// control message 통해 현재 client 수와 몇번째 msg인지 확인
	msgrcv(qid, &msg1, sizeof(msg1) - sizeof(long), 999, 0);
	cnum = msg1.cnum;
    index = msg1.s_id;
    msg1.s_id+=1;
	msgsnd(qid, &msg1, 520, 0);
	
    while(1){
        scanf("%s", temp);

        // s_id를 mtype으로 해서 보내기
        // 각 msg 마다 unique한 mtype임.
		msg1 = nmessage(index, id, temp);
               	
		// (a) message 보내기
        for(i=0; i<cnum; i++) {
            msgsnd(qid, &msg1, 520, 0);
		}
		
        if (strcmp(temp, "talk_quit")==0)
            break;
		
		index++;
    }

    exit(0);
}

// 받아야하는 mtype이 index값임
void do_receiver(int qid, int id, int index){
    struct q_entry msg1;

    while(1){
        // (b) message 받기
        // 현재 index 번째 msg를 받아야하므로 mtype이 index 값인 msg 받기
        msgrcv(qid, &msg1, 520, index, 0);

        if (msg1.s_id != id){
            printf("[sender=%d & msg#=%d] %s\n", msg1.s_id, msg1.mtype, msg1.msg);
        }
		// (c) message 받은 후 필요한 작업
		else if(msg1.s_id == id){
            printf("mine!");
            if(strcmp(msg1.msg, "talk_quit") == 0)
                break;
        }

        index++;
    }
	exit(0);
}

int main(int argc, char** argv){
    int i, qid, id, index;
    key_t key;
	
	//  필요한 변수 추가
	int pid1, pid2;
	int cnum;
	
    struct q_entry msg1, msg2;

    key = ftok("key", 5);
        
	// (f) message queue 만들고 초기화 작업
	qid = msgget(key, 0600 | IPC_CREAT | IPC_EXCL);
    if(qid == -1) { // 이미 있다면 그냥 열기
        qid = msgget(key, 0600); 
    }else{ // 없으면 만들고 cmessage 처음이자 마지막으로 생성
        msg1 = cmessage(999, 0);
        msgsnd(qid, &msg1, 520, 0);
    }
	
	id = atoi(argv[1]);
	index = 1;

    // (g) message 통신 전 필요한 작업
    // get cmessage 받고 1 증가시킨 다음 다시 넣어놓기
	msgrcv(qid, &msg1, sizeof(msg1) - sizeof(long), 999, 0);
	msg1 = cmessage(999, msg1.cnum + 1);
    // 내가 새 클라이언트로 들어왔을때 내가 지금부터 do_receiver로 
    // 받아야하는 메시지 번호
	index = msg1.s_id;
	msgsnd(qid, &msg1, 520, 0);

	// (d) 함수 호출해서 message 주고 받기	
	pid1 = fork();
    if(pid1 == 0) {
        do_sender(qid, id);
        exit(0);
    }
    pid2 = fork();
    if(pid2 == 0){
        do_receiver(qid, id, index);
        exit(0);
    }

    for(i=0;i<2;i++){
		wait(0);
    }

    // (h) message 통신 완료 후 message queue 지우기
    // cnum - 1 한거 다시 집어넣기
	msgrcv(qid, &msg1, 520, 0, 0);
    msg1.cnum -=  1;
    msgsnd(qid, &msg1, 520, 0);

	if(msg1.cnum == 0) 
		msgctl(qid, IPC_RMID, NULL);

    exit(0);
}