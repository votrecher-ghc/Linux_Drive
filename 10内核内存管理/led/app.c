#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "leddrv.h"

#define PATH "/dev/myled"


int main(int argc ,char **argv){

    char *path = PATH;

    int onoff = 0;
    int no = 0;

    sscanf(argv[1],"%d",&onoff);
    sscanf(argv[2],"%d",&no);

    int fd = -1;

    if(no < 2 || no > 5){
        printf("ledno is invalid\n");
        return -1;
    }

    fd = open(path,O_RDONLY);
    if(fd < 0){
        printf("open %s failed\n",path);
        return -1;
    }

    if(onoff){
        ioctl(fd,MY_LED_ON,no);
    }
    else{
        ioctl(fd,MY_LED_OFF,no);
    }
    close(fd);

    fd = -1;
    return 0;
}