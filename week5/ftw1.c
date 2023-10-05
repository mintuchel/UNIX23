#include <sys/stat.h>
#include <ftw.h>

int list(const char* name, const struct stat* status, int type){
    // if file
    if(type == FTW_F)
        printf("F % -30s\n", name);

    // if directory
    if(type == FTW_D)
        printf("D % -30s\n", name);

    // if read access permission is not allowed
    if(type == FTW_DNR)
        printf("DNR % -30s\n", name);

    // if symbolic link file
    if(type == FTW_SL)
        printf("SL % -30s\n", name);

    // if not supported type
    // if cant identify the file type
    if(type == FTW_NS)
        printf("NS % -30s\n", name);
        
    return 0;
}

int main(){
    // 탐색 파일 = "."
    // 실행할 함수명 = list
    // 1 = one by one
    ftw(".", list, 1);
    return 0;
}