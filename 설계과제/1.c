#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

void cat_m(char **res){
        char buf[512]; // buffer 크기는 512 bytes 입니다. 이 부분은 수정하지 마세요. 
        int fd, n;

      	fd = open(res[1], O_RDONLY);
	if(fd==-1){
		return;
	}

	while((n = read(fd, buf, sizeof(buf)))>0) {
		write(1, buf, n);
	}
}

void cd_m(char **res){
	chdir(res[1]);
}

void cp_m(char **res){
        char buf[512]; // buffer 크기는 512 bytes 입니다. 이 부분은 수정하지 마세요. 
	int fd1, fd2, n;
        struct stat s;
        
      	fd1 = open(res[1], O_RDONLY);
	if(fd1==-1) return;
	
	stat(res[1], &s);

	fd2 = open(res[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	while((n = read(fd1, buf, sizeof(buf))) > 0 ){
		write(fd2, buf, n);
	}
}

void ls_m(char **res){
	char f[512];
	DIR* dp;
	struct dirent *entry;
	struct stat buf;
 
	// open current directory
	dp = opendir(".");
	
	while((entry = readdir(dp)) != NULL){
		stat(entry->d_name, &buf);
			
		if(S_ISDIR(buf.st_mode)) { printf("D "); }
		else if(S_ISREG(buf.st_mode)) { printf("F "); }

		printf("%o %d %d %d %d %s %s\n", buf.st_mode&0777, buf.st_nlink, buf.st_uid, buf.st_gid, buf.st_size, ctime(&buf.st_mtime), entry->d_name);
	}

	/*
	printf("0%o ", buf.st_mode&077);
	printf("%d ", buf.st_nlink);
	printf("%ld ", buf.st_uid);
	printf("%ld ", buf.st_gid);
	printf("%ls ", buf.st_size);
	
	strcpy(f, ctime(&s.st_time);
	n = strlen(f);
	f[n-1] = '\0';
	printf("%s ",f);
	printf("%s\n", entry->d_name);
	*/
	closedir(dp);
	return;
}

void mkdir_m(char** res) {
	mkdir(res[1], 0700);
}


void vi_m(char **res){     
        char buf[512]; // buffer 크기는 512 bytes 입니다. 이 부분은 수정하지 마세요. 
        int fd, n;

	fd = open(res[1], O_RDONLY | O_CREAT, 0600);
	while((n=read(fd, buf, sizeof(buf)))>0) {
		buf[n] = '\0';
		if(strcmp(buf, "quit\n")==0) 
			break;
		write(fd, buf, n);
	}
}

int main(void){               
        char in[100], *res[20]={0};
        char *inst[6]={"cat", "cd", "cp", "ls", "mkdir", "vi"};
        void (*f[6])(char **)={cat_m, cd_m, cp_m, ls_m, mkdir_m, vi_m};
        int i;

        while (1){

		// current working directory 이름 출력 (미작성 시 –2점 감점)
		getcwd(in, sizeof(in));
                printf("%s> ", in);

        	gets(in);
		if (in[0]=='\0')
			continue;

		i=0;
          	res[i]=strtok(in, " ");
		while (res[i]){
			i++;
			res[i]=strtok(NULL, " ");
		}

        	if (strcmp(res[0], "exit")==0)
        	         exit(0);

        	for (i=0;i<6;i++){
        	         if (!strcmp(res[0], inst[i])){
        		         f[i](res);
        		         break;
			}
        	}
	}

        return 0;
}