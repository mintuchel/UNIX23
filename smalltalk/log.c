#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#define LOG_FILE_NAME "chat_log.txt"

struct q_entry
{
    long mtype;
    int snum;
    int cnum;
    int s_id;
    char msg[512];
};

struct manage_buffer
{
    int message_id;
    int client_num;
};

void write_log(int qid, const char * log_file_name){

    struct q_entry msg;

    // log는 모든 채팅을 다 기록해야하므로 1번부터 시작해야함
    int id_counter = 1;

    // 파일 열기 (쓰기 모드, 없으면 생성)
    FILE *log_file = fopen(log_file_name, "a");
    if (log_file == NULL)
    {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        msgrcv(qid, &msg, 524, id_counter, 0);
        
        // 로그 파일에 기록
        fprintf(log_file, "[sender=%d & msg#=%d] %s\n", msg.s_id, msg.mtype, msg.msg);
        fflush(log_file); // 버퍼 비우기, 실시간 기록 보장

        id_counter++;
    }

    // 파일 닫기 (프로그램 종료 시)
    fclose(log_file);

    exit(0);
}

// 이 프로그램은 채팅방을 개설한 사람만 실행함
int main(int argc, char **argv)
{
    // 전달받은 문자열을 int로 변환
    int shmid = atoi(argv[1]);
    int semid = atoi(argv[2]);

    int i, qid, id, index;
    pid_t pid;
    key_t log_key;

    struct q_entry msg1, msg2;
    struct manage_buffer* manager;

    log_key = ftok("logkey", 7);

    /* (k) message queue 만들고 초기화 작업 */
    qid = msgget(log_key, 0600 | IPC_CREAT | IPC_EXCL);
    if (qid == -1)
    {
        qid = msgget(log_key, 0600);
    }

    manager = (struct manage_buffer *)shmat(shmid, 0, 0);

    // 로그 파일 생성
    FILE *log_file = fopen(LOG_FILE_NAME, "w");
    if (log_file == NULL)
    {
        perror("Failed to create log file");
        exit(EXIT_FAILURE);
    }
    fclose(log_file); // 파일 닫기 (write_log에서 다시 열 예정)

    // write_log 실행하는 자식 프로세스 생성
    pid = fork();
    if (pid == 0)
    {
        write_log(qid, LOG_FILE_NAME);
        exit(0);
    }

    // 자식 프로세스 종료될때까지 부모는 wait
    wait(0);

    // 메시지 큐 제거
    msgctl(qid, IPC_RMID, NULL);

    exit(0);
}