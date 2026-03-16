#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "fs4412_mpu6050.h"

#define PATH "/dev/fs4412-mpu6050"

int main(int argc,char **argv){

    int ret = 0;
    int fd = -1;

    union mpu6050_data data;
    struct mpu6050_all_data all_data;
    
    fd = open(PATH,O_RDWR);
    if(fd < 0){
        printf("open %s failed\n",PATH);
        return -1;
    }

    while(1){
        sleep(1);

        ioctl(fd,GET_ACCEL,&data);
        printf("Union Accel-x = %d\n",data.accel.x); 
        printf("Union Accel-y = %d\n",data.accel.y); 
        printf("Union Accel-z = %d\n",data.accel.z);

        ioctl(fd,GET_GYRO,&data);
        printf("Union Gyro-x = %d\n",data.gyro.x); 
        printf("Union Gyro-y = %d\n",data.gyro.y); 
        printf("Union Gyro-z = %d\n",data.gyro.z);

        ioctl(fd,GET_TEMP,&data); 
        printf("Union Temp = %.2f C\n", data.temp.temp / 340.0 + 36.53); 

        putchar('\n');

        ioctl(fd,GET_ALL,&all_data);
        printf("ALL Accel-x = %d\n",all_data.accel.x); 
        printf("ALL Accel-y = %d\n",all_data.accel.y); 
        printf("ALL Accel-z = %d\n",all_data.accel.z);

        printf("ALL Gyro-x = %d\n",all_data.gyro.x); 
        printf("ALL Gyro-y = %d\n",all_data.gyro.y); 
        printf("ALL Gyro-z = %d\n",all_data.gyro.z);

        printf("ALL Temp = %.2f C\n", all_data.temp.temp / 340.0 + 36.53); 
        printf("===================================================\n");
    }
    close(fd);
    fd = -1;
    return 0;
}