#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(){

    // if file exits...
    if(access("data1",F_OK) == 0){
        
        // if file is readable or writable...
        if(access("data",R_OK | W_OK)){
            printf("File exists: Read/Write operations are permitted\n");  
        }else{
            printf("File exists: Read/Write operations are not permitted\n");
        }

    }else{

        printf("No such file or directory...\n");
    
    }

    return 0;
}