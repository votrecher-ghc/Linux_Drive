#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "mpu6050.h"

#define PATH "/dev/mpu6050"

int main(int argc,char **argv){

    char *path = PATH;
    int fd = -1;
    union mpu6050_data mpu6050_data;
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
        sleep(2);

        ioctl(fd,GET_ACCEL,&mpu6050_data); 
        printf("Accel-x = %d\n",mpu6050_data.accel.x); 
        printf("Accel-y = %d\n",mpu6050_data.accel.y); 
        printf("Accel-z = %d\n",mpu6050_data.accel.z);   

        ioctl(fd,GET_GYRO,&mpu6050_data); 
        printf("Gyro-x = %d\n",mpu6050_data.gyro.x); 
        printf("Gyro-y = %d\n",mpu6050_data.gyro.y); 
        printf("Gyro-z = %d\n",mpu6050_data.gyro.z) ;

        ioctl(fd,GET_TEMP,&mpu6050_data); 
        printf("Temp = %.2f C\n", mpu6050_data.temp / 340.0 + 36.53); 

        putchar('\n');
    }

    close(fd);
    fd = -1;
    return 0;
}