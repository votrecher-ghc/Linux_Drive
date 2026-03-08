# 变量定义
CC      = arm-none-linux-gnueabi-gcc
CFLAGS  = -Wall -g
TARGET  = mpu6050_test
SRCS    = main.c mpu6050_op_rw.c
OBJS    = $(SRCS:.c=.o)

# 默认目标
all: $(TARGET)

# 链接目标文件
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# 编译源文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean