#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>


#define PATH "/dev/input/event1"

int main(int argc, char **argv) {
    int fd = -1;
    int ret = 0;
    struct input_event evt;

    fd = open(PATH, O_RDWR);
    if(fd < 0){
        printf("open %s failed\n", PATH);
        return -1;
    }

    printf("App starting... listening to %s\n", PATH);

    while(1){
        /* 阻塞读取事件，如果没有按键按下，程序会在这里休眠等待，不占CPU */
        ret = read(fd, &evt, sizeof(evt));
        
        // 确保读到了一个完整的事件结构体
        if(ret != sizeof(evt)){
            continue; 
        }

        /* 第一层过滤：只关心按键类事件 (排除掉 EV_SYN 等其他事件) */
        if(evt.type == EV_KEY){
            /* 第二层路由：判断具体是哪个按键 */
            switch(evt.code){
                case KEY_2:
                    /* 第三层判断：判断按键的状态 (1是按下，0是松开) */
                    if(evt.value == 1){
                        printf(">>> KEY_2 Down (Pressed)\n");
                    }else if(evt.value == 0){
                        printf("<<< KEY_2 Up (Released)\n");
                    }
                    break;

                case KEY_3:
                    if(evt.value == 1){
                        printf(">>> KEY_3 Down (Pressed)\n");
                    }else if(evt.value == 0){
                        printf("<<< KEY_3 Up (Released)\n");
                    }
                    break;

                case KEY_4:
                    if(evt.value == 1){
                        printf(">>> KEY_4 Down (Pressed)\n");
                    }else if(evt.value == 0){
                        printf("<<< KEY_4 Up (Released)\n");
                    }
                    break;
                    
                default:
                    // 抓取到未知的按键
                    printf("Unknown Key Code: %d\n", evt.code);
                    break;
            }
        }
    }

    close(fd);
    fd = -1;
    return 0;
}