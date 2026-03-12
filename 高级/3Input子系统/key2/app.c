#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>

#define PATH "/dev/input/event1"

int main(int argc,char **argv){

    char *path = PATH;
    int fd = -1;
    struct input_event evt;
   
    /**/
    if(argc > 2){
        printf("argument is too few ,Usage: %s dev\n",argv[0]);
        return -1;
    }
    if(argc == 2){
        path = argv[1];
    }


    fd = open(path,O_RDONLY);
    if(fd < 0){
        printf("open failed\n");
        return -1;
    }

    while(1){
        read(fd, &evt, sizeof(evt));
        if(evt.type == EV_KEY && evt.code == KEY_2){
            if(evt.value){
                printf("KEY 2 Down\n");
            }else{
                printf("KEY 2 Up\n");
            }
        }

    }

    close(fd);
    fd = -1;
    return 0;
}