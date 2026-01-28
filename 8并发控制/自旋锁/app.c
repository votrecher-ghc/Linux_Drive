#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define PATH "/dev/openonce"
int main(int argc ,char **argv){

    char *path = PATH;
    if(argc == 2){
        path = argv[1];
    }

    int fd = -1;
    fd = open(path,O_RDWR);

    if(fd< 0){
        printf("open %s failed \n",path);
        return -1;
    }

    while(1);

    close(fd);
    fd = -1;
    
    return 0;
}