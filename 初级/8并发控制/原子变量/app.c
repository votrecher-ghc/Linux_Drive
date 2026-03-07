#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc,char **argv){
    int fd = -1;
    char *path = "/dev/openonce";
    if(argc == 2){
        path = argv[1];
    }

    fd = open(path,O_RDWR);

    if(fd < 0){
        printf("open %s failed\n",path);
        return -1;
    }

    while(1);
    return 0;
}