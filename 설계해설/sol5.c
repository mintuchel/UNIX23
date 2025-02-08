#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <ftw.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/errno.h>

#define BUFSIZE 512
#define N 100

// 공유메모리에 대한 정보
struct manager
{
    int front_idx; // 원형큐 머리
    int rear_idx; // 원형큐 꼬리
    int nextId; // 다음 메시지 id
    int cnum; // 현재 접속인원수
};

// 메시지 정보
struct message
{
    int read_counter; // 몇 명이 읽어야하는지
    int message_id; // 내가 몇번째 메시지인지
    int sender_id; // 보낸 사람 id
    char mtext[BUFSIZE]; // message 내용
};

void receiver(int id, int message_id, int semid, int manager_shmid, int message_arr_shmid)
{
    // 1번째에 -1 하여 읽을 메세지가 존재하는지 확인
    // 1번째가 0이면 모든 메세지를 읽었다는 뜻이므로 새 메세지가 들어올때까지 BLOCK
    // 2번째에 -1하여 공유자원에 대한 베타적 접근 여부 확인
    // 2번째가 0이면 누가 사용하고 있다는 것이므로 BLOCK
    struct sembuf wait[] = {{1, -1, 0}, {2, -1, 0}};

    // 0번째에 +1하여 버퍼 공간 추가 신호
    // 2번째에 +1하여 공유자원에 대한 접근 권한 반환
    struct sembuf signal1[] = {{0, 1, 0}, {2, 1, 0}};

    // 내가 읽을 메시지가 아니면 read 권한과 접근 권한만 반납해놓으면 됨
    // wait 때 -1 한 것을 그대로 +1 시켜서 놓으면 됨
    struct sembuf signal2[] = {{1, 1, 0}, {2, 1, 0}};

    struct manager *manager_shm;
    struct message *message_arr_shm;

    manager_shm = (struct manager *)shmat(manager_shmid, 0, 0);
    message_arr_shm = (struct message *)shmat(message_arr_shmid, 0, 0);

    while (1)
    {
        // 읽을 메세지 있는지 확인
        semop(semid, wait, 2);

        // 현재 처리해야하는 id 와 메시지의 id가 동일하다면 읽기
        if (message_id == message_arr_shm[manager_shm->front_idx].message_id)
        {
            // 내가 보낸게 아니라면 출력
            if(message_arr_shm[manager_shm->front_idx].sender_id != id){
                printf("[receiver] %d : %s\n", message_arr_shm[manager_shm->front_idx].sender_id, message_arr_shm[manager_shm->front_idx].mtext);
            }

            // 메시지 읽은 사람 -1
            message_arr_shm[manager_shm->front_idx].read_counter--;
            // 다음으로 읽어야할 message_id +1
            message_id++;

            // 모든 수신자가 해당 메시지를 읽었다면
            if (message_arr_shm[manager_shm->front_idx].read_counter == 0) {
                // 원형큐의 frontIdx를 다음 위치로 이동
                manager_shm->front_idx = (manager_shm->front_idx + 1) % N;
                // 버퍼 공간 확보 신호 + 공유 자원 접근 권한 반환
                semop(semid, signal1, 2);
                continue;
            }
        }

        // 내가 읽어야하는 message_id가 아니면 or 메시지를 읽었는데 마지막으로 읽은 사람이 아니면
        // read에 대한 권한 반환 + 공유 자원 접근 권한 반환
        semop(semid, signal2, 2);
    }
    return;
}

void sender(int id, int semid, int manager_shmid, int message_arr_shmid)
{
    // 0번째에 -1 하여 버퍼에 메시지를 저장할 공간이 있는지 확인
    // 0번째가 0이면 버퍼에 빈 공간이 없다는 뜻이므로 BLOCK
    // 2번째에 -1하여 공유메모리에 대한 접근 가능 여부 확인
    // 2번째가 0이면 누가 사용하고 있다는 것이므로 BLOCK
    struct sembuf wait[] = {{0, -1, 0}, {2, -1, 0}};

    // 1번째에 +1하여 읽을 메시지가 추가되었다고 signal
    // 2번째에 +1하여 공유자원에 대한 접근 권한 반환
    struct sembuf signal[] = {{1, 1, 0}, {2, 1, 0}};

    struct manager *manager_shm;
    struct message *message_arr_shm;

    manager_shm = (struct manager *)shmat(manager_shmid, 0, 0);
    message_arr_shm = (struct message *)shmat(message_arr_shmid, 0, 0);

    char input[BUFSIZE];

    while (gets(input))
    {
        // semwait 작업
        semop(semid, wait, 2);

        // message 버퍼에 저장하기
        message_arr_shm[manager_shm->rear_idx].sender_id = id;
        strcpy(message_arr_shm[manager_shm->rear_idx].mtext, input);
        message_arr_shm[manager_shm->rear_idx].message_id = manager_shm->nextId;
        message_arr_shm[manager_shm->rear_idx].read_counter = manager_shm->cnum;
        manager_shm->nextId++;
        // 내 메시지 저장했으니 다음 메시지 저장될 곳 갱신
        // 원형큐이므로 버퍼 내에서 계속 순환하며 저장
        manager_shm->rear_idx = (manager_shm->rear_idx + 1) % N;

        // semsignal
        semop(semid, signal, 2);
    }

    return;
}

// sempahore 3개
// shared memory 2개 사용
int main(int argc, char **argv)
{
    int i, id;
    int semid, manager_shmid, message_arr_shmid;
    key_t semkey, manager_shmkey, message_arr_shmkey;
    pid_t pid[2];

    // 세마포어 집합 초기화시 사용할 semun
    union semun arg;

    // 세마포어 초기화 시 사용할 SETALL을 위한 int 배열
    // 1 = prevents overflow (amount of free space left in buffer)
    // 2 = prevents underflow (number of items in the buffer that are available for consumption)
    // 3 = control buffer access (mutual exclusion)
    ushort buf[3] = {N, 0, 1};

    struct manager *manage;
    struct message *message;

    // 필요한 IPC 객체 키 가져오기
    semkey = ftok("semkey", 3);
    manager_shmkey = ftok("manage_shmkey", 3);
    message_arr_shmkey = ftok("message_shmkey", 3);

    // sempahore 3개 생성
    semid = semget(semkey, 3, 0600 | IPC_CREAT | IPC_EXCL);
    // 이미 존재한다면 기존꺼 열기
    if (semid == -1)
    {
        semid = semget(semkey, 3, 0600);
    }
    // 생성에 성공했으면 없었다는 거임
    // semaphore 초기화 해주기
    else
    {
        // semun 과 buf 사용해서 SETALL로 사용할 세마포어 전부 초기화해주기
        arg.array = buf;
        semctl(semid, 0, SETALL, arg);
    }

    // manage_sharedmemory 생성/참조
    manager_shmid = shmget(manager_shmkey, sizeof(struct manager), 0600 | IPC_CREAT);
    manage = (struct manager *)shmat(manager_shmid, 0, 0);

    // 톡방을 판 사람이면 초기화 진행
    if(manage->cnum == 0){
        manage->front_idx = 0;
        manage->rear_idx = 0;
        manage->nextId = 1;
        manage->cnum = 0;
    }

    // N개의 message_buffer 공간을 확보한 shared_memory 생성/참조
    message_arr_shmid = shmget(message_arr_shmkey, N * sizeof(struct message), 0600 | IPC_CREAT);
    message = (struct message *)shmat(message_arr_shmid, 0, 0);

    id = atoi(argv[1]);

    struct sembuf w_buf = {2, -1, 0};
    struct sembuf s_buf = {2, 1, 0};

    int message_id;

    semop(semid, &w_buf, 1);

    manage->cnum += 1; // 공유메모리 사용자 + 1
    message_id = manage->nextId; // 읽어야할 메시지 수 파악

    semop(semid, &s_buf, 1);

    printf("id : %d\n", id);

    pid[0] = fork();
    if(pid[0]==0){
        receiver(id, message_id, semid, message_id, manager_shmid);
        exit(0);
    }

    pid[1] = fork();
    if(pid[1]==0){
        sender(id, semid, manager_shmid, message_arr_shmid);
        exit(0);
    }

    for (int i = 0; i < 2;i++)
        wait(0);

    // 특정 사용자가 종료했으면
    // 공유메모리 총 사용자 수 1 감소
    manage->cnum -= 1;

    // 만약 한명도 없으면 IPC 객체 모두 삭제
    if (manage->cnum == 0)
    {
        semctl(semid, IPC_RMID, 0);
        shmctl(manager_shmid, IPC_RMID, 0);
        shmctl(message_arr_shmid, IPC_RMID, 0);
    }

    // 정상 종료
    exit(0);
}