#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <errno.h>
#include <string.h>

#include "fs4412_mpu6050.h"
#include "fs4412_leds.h"

#define MPU6050_PATH "/dev/fs4412-mpu6050"
#define LED_PATH     "/dev/fs4412-leds"
#define KEY_PATH     "/dev/input/event1"

#define ROLL_THRESHOLD   15.0
#define PITCH_THRESHOLD  15.0

#define MODE_DIRECTION   0
#define MODE_ALARM       1

static void led_all_off(int led_fd)
{
    ioctl(led_fd, LED_OFF, 2);
    ioctl(led_fd, LED_OFF, 3);
    ioctl(led_fd, LED_OFF, 4);
    ioctl(led_fd, LED_OFF, 5);
}

static void led_all_on(int led_fd)
{
    ioctl(led_fd, LED_ON, 2);
    ioctl(led_fd, LED_ON, 3);
    ioctl(led_fd, LED_ON, 4);
    ioctl(led_fd, LED_ON, 5);
}

static void handle_direction_mode(int led_fd, double roll, double pitch)
{
    /* 先全灭 */
    led_all_off(led_fd);

    /* 左右倾斜 */
    if (roll < -ROLL_THRESHOLD) {
        ioctl(led_fd, LED_ON, 2);
    } else if (roll > ROLL_THRESHOLD) {
        ioctl(led_fd, LED_ON, 3);
    }

    /* 前后倾斜 */
    if (pitch < -PITCH_THRESHOLD) {
        ioctl(led_fd, LED_ON, 4);
    } else if (pitch > PITCH_THRESHOLD) {
        ioctl(led_fd, LED_ON, 5);
    }
}

static void handle_alarm_mode(int led_fd, double roll, double pitch)
{
    if (fabs(roll) > ROLL_THRESHOLD || fabs(pitch) > PITCH_THRESHOLD) {
        led_all_on(led_fd);
    } else {
        led_all_off(led_fd);
    }
}

int main(void)
{
    int ret;
    int mpu_fd = -1;
    int led_fd = -1;
    int key_fd = -1;

    int mode = MODE_DIRECTION;

    struct mpu6050_all_data all_data;
    struct input_event evt;

    double roll_raw, pitch_raw;
    double roll, pitch;

    double roll_offset = 0.0;
    double pitch_offset = 0.0;

    mpu_fd = open(MPU6050_PATH, O_RDWR);
    if (mpu_fd < 0) {
        perror("open mpu6050");
        return -1;
    }

    led_fd = open(LED_PATH, O_RDWR);
    if (led_fd < 0) {
        perror("open leds");
        close(mpu_fd);
        return -1;
    }

    key_fd = open(KEY_PATH, O_RDONLY | O_NONBLOCK);
    if (key_fd < 0) {
        perror("open key input");
        close(mpu_fd);
        close(led_fd);
        return -1;
    }

    printf("姿态检测程序启动成功\n");
    printf("KEY_2: 切换模式\n");
    printf("KEY_3: 当前姿态校准为零点\n");
    printf("KEY_4: 清空 LED\n");

    led_all_off(led_fd);

    while (1) {
        /* 1. 读取 MPU6050 数据 */
        ret = ioctl(mpu_fd, GET_ALL, &all_data);
        if (ret < 0) {
            perror("ioctl GET_ALL");
            break;
        }

        /* 2. 计算原始 Roll / Pitch */
        {
            double ax = (double)all_data.accel.x;
            double ay = (double)all_data.accel.y;
            double az = (double)all_data.accel.z;

            roll_raw  = atan2(ay, az) * 180.0 / M_PI;
            pitch_raw = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / M_PI;
        }

        /* 3. 扣除零点偏移 */
        roll  = roll_raw  - roll_offset;
        pitch = pitch_raw - pitch_offset;

        /* 4. 处理按键事件（非阻塞） */
        while (1) {
            ret = read(key_fd, &evt, sizeof(evt));
            if (ret < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                    break;
                perror("read key");
                goto out;
            }

            if (ret != sizeof(evt))
                break;

            if (evt.type == EV_KEY && evt.value == 1) {
                switch (evt.code) {
                case KEY_2:
                    mode = (mode == MODE_DIRECTION) ? MODE_ALARM : MODE_DIRECTION;
                    printf("[KEY_2] 切换模式 -> %s\n",
                           mode == MODE_DIRECTION ? "方向显示模式" : "超阈值报警模式");
                    break;

                case KEY_3:
                    roll_offset = roll_raw;
                    pitch_offset = pitch_raw;
                    printf("[KEY_3] 校准成功，当前姿态设为零点\n");
                    break;

                case KEY_4:
                    led_all_off(led_fd);
                    printf("[KEY_4] 已清空 LED\n");
                    break;

                default:
                    printf("[KEY] 未处理按键 code=%d\n", evt.code);
                    break;
                }
            }
        }

        /* 5. 根据模式控制 LED */
        if (mode == MODE_DIRECTION) {
            handle_direction_mode(led_fd, roll, pitch);
        } else {
            handle_alarm_mode(led_fd, roll, pitch);
        }

        /* 6. 打印数据 */
        printf("Raw Roll = %7.2f, Raw Pitch = %7.2f | ", roll_raw, pitch_raw);
        printf("Cal Roll = %7.2f, Cal Pitch = %7.2f | ", roll, pitch);
        printf("Mode = %s\n", mode == MODE_DIRECTION ? "DIRECTION" : "ALARM");

        usleep(200000);
    }

out:
    led_all_off(led_fd);

    close(key_fd);
    close(mpu_fd);
    close(led_fd);

    return 0;
}