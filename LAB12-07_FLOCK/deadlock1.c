#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

// p1은 n1 lock 후 n2 lock

int main(){
	int fd, n1, n2, num1, num2, res1, res2;
	struct flock lock;

	fd = open("data1", O_RDWR | O_CREAT, 0600);
	
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = sizeof(int);
	printf("%d\n", n1 = fcntl(fd, F_SETLKW, &lock));

	sleep(1);

	// if n1 lock success
	if(n1==0) {
		lock.l_type = F_WRLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = sizeof(int);
		lock.l_len = sizeof(int);
		printf("%d\n", n2 = fcntl(fd, F_SETLKW, &lock));

		// if n2 lock success	
		if(n2==0) {
			read(fd, &num1, sizeof(int));
			read(fd, &num2, sizeof(int));
			int res1 = num1 + num2;
			int res2 = num2 - num1;
			printf("%d %d %d %d\n", num1, num2, res1, res2);

			sleep(1);

			lseek(fd, 0, SEEK_SET);
			write(fd, &res1, sizeof(int));
			write(fd, &res2, sizeof(int));

			lock.l_type = F_UNLCK;
			lock.l_whence = SEEK_SET;
			lock.l_start = sizeof(int);
			lock.l_len = sizeof(int);
			fcntl(fd, F_SETLK, &lock);
		}

		lock.l_type = F_UNLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = 0;
		lock.l_len = sizeof(int);
		fcntl(fd, F_SETLK, &lock);
	}
	return 0;
}