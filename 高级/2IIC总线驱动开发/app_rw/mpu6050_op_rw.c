#include "mpu6050.h"

/**
 * fd:文件描述符
 * reg：寄存器编号
 * pdata：读取数据存放地址
 */
static int read_data_from_mpu6050(int fd,unsigned char reg,unsigned char* pdata){
    int ret = 0;
    unsigned char buf[1] = {reg};

    /**
     * write一被调用
     * 驱动会立刻在总线上产生：START 信号 + 从机地址 + W(写位)
     * 调用结束时会发出STOP信号
     * 虽然总线停止了，但 MPU6050 内部的硬件逻辑已经把 reg 的值存进了它的“指针”里。
     * 这个指针在掉电前或被下次修改前，会一直指着这个位置。
     */
    /*设置读取哪个寄存器*/
    ret = write(fd,buf,1);
    if(ret != 1){
        printf("write data in read_data_from_mpu6050 failed\n");
        return -1;
    }

    /**
     * 总线发出：START -> 从机地址+R。
     * 此时从设备收到“读”请求，它会直接去查看自己内部那个“地址指针”。
     * 发现指针指着 reg，于是它就把 reg 里的数据吐出来。
     * 最后发出 STOP
     */
    /*设置完成之后，直接读取fd即可*/
    buf[0] = 0;
    ret = read(fd,buf,1);
    if(ret != 1){
        printf("read data in read_data_from_mpu6050 failed\n");
        return -1;
    }
    *pdata = buf[0];
    return 0;
}

static int write_data_to_mpu6050(int fd,unsigned char reg,unsigned char data){
    
    int ret = 0;
    unsigned char buf[2] = {reg,data};

    ret = write(fd,buf,2);
    if(ret != 2){
        printf("write data in write_data_to_mpu6050 failed\n");
        return -1;
    }
    
    return 0;
}


int init_mpu6050(int fd){

    int ret = 0;

    ret = ioctl(fd,I2C_TENBIT,0);
    if(ret < 0){
        printf("ioctl I2C_TENBIT failed in init init_mpu6050");
        return -1;
    }
    ret = ioctl(fd,I2C_SLAVE,0x68);
    if(ret < 0){
        printf("ioctl I2C_SLAVE failed in init init_mpu6050");
        return -1;
    }

    ret = write_data_to_mpu6050(fd,PWR_MGMT_1,0x00);
    ret += write_data_to_mpu6050(fd,SMPLRT_DIV,0x07);
    ret += write_data_to_mpu6050(fd,ACCEL_CONFIG,0x19);
    ret += write_data_to_mpu6050(fd,GYRO_CONFIG,0xF8);

    if(ret < 0){
        printf("write init data to mpu6050 failed\n");
        return -1;
    }
    
    return 0;
}


int read_accelx(int fd){
    unsigned short val = 0;
    unsigned char d = 0;
    int ret = 0;

    ret = read_data_from_mpu6050(fd,ACCEL_XOUT_L,&d);
    val = d;
    ret = read_data_from_mpu6050(fd,ACCEL_XOUT_H,&d);
    val |= d << 8;

    if(ret < 0){
        printf("read accel x value failed in read_accelx\n");
        return -1;
    }
    else{
        return val;
    }
}

int read_accely(int fd){
    unsigned short val = 0;
    unsigned char d = 0;
    int ret = 0;

    ret = read_data_from_mpu6050(fd,ACCEL_YOUT_L,&d);
    val = d;
    ret = read_data_from_mpu6050(fd,ACCEL_YOUT_H,&d);
    val |= d << 8;

    if(ret < 0){
        printf("read accel y value failed in read_accely\n");
        return -1;
    }
    else{
        return val;
    }
}

int read_accelz(int fd){
    unsigned short val = 0;
    unsigned char d = 0;
    int ret = 0;

    ret = read_data_from_mpu6050(fd,ACCEL_ZOUT_L,&d);
    val = d;
    ret = read_data_from_mpu6050(fd,ACCEL_ZOUT_H,&d);
    val |= d << 8;

    if(ret < 0){
        printf("read accel z value failed in read_accelz\n");
        return -1;
    }
    else{
        return val;
    }
}

int read_temp(int fd){
    unsigned short val = 0;
    unsigned char d = 0;
    int ret = 0;

    ret = read_data_from_mpu6050(fd,TEMP_OUT_L,&d);
    val = d;
    ret = read_data_from_mpu6050(fd,TEMP_OUT_H,&d);
    val |= d << 8;

    if(ret < 0){
        printf("read temp value failed in read_temp\n");
        return -1;
    }
    else{
        return val;
    }
}

int read_gyrox(int fd){
    unsigned short val = 0;
    unsigned char d = 0;
    int ret = 0;

    ret = read_data_from_mpu6050(fd,GYRO_XOUT_L,&d);
    val = d;
    ret = read_data_from_mpu6050(fd,GYRO_XOUT_H,&d);
    val |= d << 8;

    if(ret < 0){
        printf("read gyro x value failed in read_read_gyrox\n");
        return -1;
    }
    else{
        return val;
    }
}

int read_gyroy(int fd){
    unsigned short val = 0;
    unsigned char d = 0;
    int ret = 0;

    ret = read_data_from_mpu6050(fd,GYRO_YOUT_L,&d);
    val = d;
    ret = read_data_from_mpu6050(fd,GYRO_YOUT_H,&d);
    val |= d << 8;

    if(ret < 0){
        printf("read gyro y value failed in read_read_gyroy\n");
        return -1;
    }
    else{
        return val;
    }
}

int read_gyroz(int fd){
    unsigned short val = 0;
    unsigned char d = 0;
    int ret = 0;

    ret = read_data_from_mpu6050(fd,GYRO_ZOUT_L,&d);
    val = d;
    ret = read_data_from_mpu6050(fd,GYRO_ZOUT_H,&d);
    val |= d << 8;

    if(ret < 0){
        printf("read gyro z value failed in read_read_gyroz\n");
        return -1;
    }
    else{
        return val;
    }
}