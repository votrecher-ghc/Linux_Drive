#ifndef MPU6050_H
#define MPU6050_H

#include <linux/ioctl.h>

#define SMPLRT_DIV		0x19
#define CONFIG			0x1A
#define GYRO_CONFIG		0x1B
#define ACCEL_CONFIG	0x1C
#define ACCEL_XOUT_H	0x3B
#define ACCEL_XOUT_L	0x3C
#define ACCEL_YOUT_H	0x3D
#define ACCEL_YOUT_L	0x3E
#define ACCEL_ZOUT_H	0x3F
#define ACCEL_ZOUT_L	0x40
#define TEMP_OUT_H		0x41
#define TEMP_OUT_L		0x42
#define GYRO_XOUT_H		0x43
#define GYRO_XOUT_L		0x44
#define GYRO_YOUT_H		0x45
#define GYRO_YOUT_L		0x46
#define GYRO_ZOUT_H		0x47
#define GYRO_ZOUT_L		0x48
#define PWR_MGMT_1		0x6B


struct accel_data{
    short x;
    short y;
    short z;
};

struct gyro_data{
    short x;
    short y;
    short z;
};
struct temp_data{
    short temp;

};

union mpu6050_data{
    struct accel_data accel;
    struct gyro_data gyro;
    struct temp_data temp;
};

struct mpu6050_all_data{
    struct accel_data accel;
    struct gyro_data gyro;
    struct temp_data temp;
};

#define MPU6050_MAGIC 'K'

#define GET_ACCEL _IOR(MPU6050_MAGIC,0,union mpu6050_data)
#define GET_GYRO _IOR(MPU6050_MAGIC,1,union mpu6050_data)
#define GET_TEMP _IOR(MPU6050_MAGIC,2,union mpu6050_data)
#define GET_ALL _IOR(MPU6050_MAGIC,3,struct mpu6050_all_data)


#endif