#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "mpu6050.h" /* 包含头文件以识别传感器操作函数 */

/**
 * @brief 主函数：初始化并循环读取 MPU6050 数据
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组，argv[1] 应为 I2C 设备节点路径（如 /dev/i2c-1）
 * @return int 状态码，0 表示正常退出
 */
int main(int argc, char **argv) {
    int fd = -1;
    int ax, ay, az, gx, gy, gz, temp;

    if (argc < 2) {
        printf("Usage: %s <device_path>\n", argv[0]);
        return 1;
    }

    /* 1. 打开 I2C 控制器设备文件 */
    fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        printf("open %s failed\n", argv[1]);
        return 2;
    }

    /* 2. 初始化 MPU6050 硬件配置 */
    if (init_mpu6050(fd) < 0) {
        printf("init_mpu6050 failed\n");
        close(fd);
        return 3;
    }

    printf("MPU6050 Initialized Successfully!\n");

    while (1) {
        /* 3. 读取各传感器原始数据 */
        ax = read_accelx(fd);
        ay = read_accely(fd);
        az = read_accelz(fd);
        temp = read_temp(fd);
        gx = read_gyrox(fd);
        gy = read_gyroy(fd);
        gz = read_gyroz(fd);

        /* 4. 打印输出结果 */
        printf("Accel: X:%6d Y:%6d Z:%6d  |  ", ax, ay, az);
        printf("Gyro: X:%6d Y:%6d Z:%6d  |  ", gx, gy, gz);
        printf("Temp: %.2f degC\n", temp / 340.0 + 36.53); /* 这里的转换公式参考手册 */

        sleep(3); /* 延时 1 秒，避免刷屏过快 */
    }

    /* 5. 关闭设备 */
    close(fd);
    fd = -1;
    return 0;
}