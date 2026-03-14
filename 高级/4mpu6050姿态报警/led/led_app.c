#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "fs4412_leds.h"

#define PATH "/dev/fs4412-leds"

int main(int argc, char **argv){

    if(argc != 3){
        printf("Argument input wrong,Usage:%s ledno status\n",argv[0]);
        return -1;
    }

    int fd = -1;
    int ledno = 0;
    int status = 0;
    int ret = 0;

    fd = open(PATH,O_RDWR);
    if(fd < 0){
        printf("open %s failed\n",PATH);
        return -1;
    }

    ledno = atoi(argv[1]);
    status = atoi(argv[2]);

    if (ledno < 2 || ledno > 5) {
        printf("invalid ledno: %d\n", ledno);
        return -1;
    }

    if (status != 0 && status != 1) {
        printf("invalid status: %d\n", status);
        return -1;
    }

    
    if (status == 1) {
        ret = ioctl(fd, LED_ON, ledno);
        if (ret < 0) {
            perror("ioctl LED_ON");
            close(fd);
            return -1;
        }
        printf("led%d on success\n", ledno);
    } else {
        ret = ioctl(fd, LED_OFF, ledno);
        if (ret < 0) {
            perror("ioctl LED_OFF");
            close(fd);
            return -1;
        }
        printf("led%d off success\n", ledno);
    }


    close(fd);
    fd = -1;
    return 0;
   
}