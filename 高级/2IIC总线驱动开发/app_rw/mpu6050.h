#ifndef MPU_6050_H
#define MPU_6050_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int init_mpu6050(int fd);
int read_accelx(int fd);
int read_accely(int fd);
int read_accelz(int fd);
int read_temp(int fd);
int read_gyrox(int fd);
int read_gyroy(int fd);
int read_gyroz(int fd);

#define SMPLRT_DIV 0x19
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C

#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48

#define PWR_MGMT_1 0x6B

#define I2C_SLAVE 0x0703    /* Use this slave address */
#define I2C_TENBIT 0x0704   /* 0 for 7 bit addrs, != 0 for 10 bit */
#define I2C_RDWR 0x0707

struct i2c_msg {
        unsigned short addr;     /* slave address                        */
        unsigned short flags;
#define I2C_M_TEN               0x0010  /* this is a ten bit chip address */
#define I2C_M_RD                0x0001  /* read data, from slave to master */
        unsigned short len;              /* msg length                           */
        unsigned char *buf;              /* pointer to msg data                  */
};

  
/* This is the structure as used in the I2C_RDWR ioctl call */
struct i2c_rdwr_ioctl_data {
          struct i2c_msg *msgs;    /* pointers to i2c_msgs */
          unsigned int nmsgs;                    /* number of i2c_msgs */
};


#endif