#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>

#include "mpu6050.h"

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


int major = 11;
int minor = 0;
int mpu6050_num = 1;

struct mpu6050_dev{
    struct cdev mydev;
    struct i2c_client *pclt;
};

struct mpu6050_dev *pgmydev = NULL;

int mpu6050_read_byte(struct i2c_client * pclt,unsigned char reg){

    int ret = 0;
    char txbuf[1] = {reg};
    char rxbuf[1] = {0};
    struct i2c_msg msg[2] = {
        {pclt->addr,0,1,txbuf},
        {pclt->addr,I2C_M_RD,1,rxbuf},
    };


    ret = i2c_transfer(pclt->adapter,msg,ARRAY_SIZE(msg));
    if(ret != ARRAY_SIZE(msg)){
        return ret;
    }

    return rxbuf[0];
}

int mpu6050_write_byte(struct i2c_client * pclt,unsigned char reg,unsigned char val){
    int ret = 0;
    char buf[2] = {reg,val};
    struct i2c_msg msg = {.addr = pclt->addr,  .flags = 0,  .len = 2, .buf = buf};
        
    ret = i2c_transfer(pclt->adapter,&msg,1);
    if(ret != 1){
        return ret;
    }

    return 0;
}

int mpu6050_open(struct inode* pnode,struct file* pfile){
    printk("mpu6050_device open\n");
    pfile->private_data = (void *)(container_of(pnode->i_cdev,struct mpu6050_dev,mydev));
    return 0;
}

int mpu6050_close(struct inode* pnode,struct file *pfile){
    printk("mpu6050_device close\n");
    return 0;
}

long mpu6050_ioctl(struct file *pfile,unsigned int cmd,unsigned long arg){
    struct mpu6050_dev* pmydev = (struct mpu6050_dev *)pfile->private_data;

    union mpu6050_data data;
    memset(&data, 0, sizeof(data));
    
    switch(cmd){
            case GET_ACCEL:
                data.accel.x = mpu6050_read_byte(pmydev->pclt,ACCEL_XOUT_L);
                data.accel.x |= mpu6050_read_byte(pmydev->pclt,ACCEL_XOUT_H) << 8;

                data.accel.y = mpu6050_read_byte(pmydev->pclt,ACCEL_YOUT_L);
                data.accel.y |= mpu6050_read_byte(pmydev->pclt,ACCEL_YOUT_H) << 8;

                data.accel.z = mpu6050_read_byte(pmydev->pclt,ACCEL_ZOUT_L);
                data.accel.z |= mpu6050_read_byte(pmydev->pclt,ACCEL_ZOUT_H) << 8;

                break;
            case GET_GYRO:
                data.gyro.x = mpu6050_read_byte(pmydev->pclt,GYRO_XOUT_L);
                data.gyro.x |= mpu6050_read_byte(pmydev->pclt,GYRO_XOUT_H) << 8;

                data.gyro.y = mpu6050_read_byte(pmydev->pclt,GYRO_YOUT_L);
                data.gyro.y |= mpu6050_read_byte(pmydev->pclt,GYRO_YOUT_H) << 8;

                data.gyro.z = mpu6050_read_byte(pmydev->pclt,GYRO_ZOUT_L);
                data.gyro.z |= mpu6050_read_byte(pmydev->pclt,GYRO_ZOUT_H) << 8;
                break;
            case GET_TEMP:
                data.temp = mpu6050_read_byte(pmydev->pclt,TEMP_OUT_L);
                data.temp |= mpu6050_read_byte(pmydev->pclt,TEMP_OUT_H) << 8;
                break;
            default:
                printk("Invalid CMD\n");
                return -EINVAL;
    }

    if(copy_to_user((void __user*)arg,&data,sizeof(data))){
        return -EFAULT;
    }

    return 0;
}

void init_mpu6050(struct i2c_client *pclt){

    mpu6050_write_byte(pclt,PWR_MGMT_1,0x00);
    mpu6050_write_byte(pclt,SMPLRT_DIV,0x07);
    mpu6050_write_byte(pclt,CONFIG,0x06);
    mpu6050_write_byte(pclt,GYRO_CONFIG,0x00);
    mpu6050_write_byte(pclt,ACCEL_CONFIG,0x00);
}

struct file_operations myops = {
    .owner = THIS_MODULE,
    .open = mpu6050_open,
    .release = mpu6050_close,
    .unlocked_ioctl = mpu6050_ioctl,
};

static int mpu6050_probe(struct i2c_client* pclt,const struct i2c_device_id *pid){
    int ret = 0;
    dev_t devno = MKDEV(major,minor);
    ret = register_chrdev_region(devno,mpu6050_num,"mpu6050");
    if(ret){
        ret = alloc_chrdev_region(&devno,minor,mpu6050_num,"mpu6050");
        if(ret){
            printk("[mpu6050_probe] alloc_chardev_region faild\n");
            return -1;
        }
        major = MAJOR(devno);
        minor = MINOR(devno);
        printk("alloc devno: %d\n",major);
    }
    printk("devno:%d\n",devno);

    pgmydev = (struct mpu6050_dev *)kmalloc(sizeof(struct mpu6050_dev),GFP_KERNEL);
    if(NULL == pgmydev){
        unregister_chrdev_region(devno,mpu6050_num);
        printk("[mpu6050_probe] kmalloc for pgmyedv failed\n");
        return -1;
    }
    memset(pgmydev,0,sizeof(struct mpu6050_dev));

    pgmydev -> pclt = pclt;
    init_mpu6050(pgmydev -> pclt);

    cdev_init(&pgmydev->mydev,&myops);
    pgmydev->mydev.owner = THIS_MODULE;
    ret = cdev_add(&pgmydev->mydev,devno,mpu6050_num);
    if(ret){
        kfree(pgmydev);
        unregister_chrdev_region(devno, mpu6050_num);
        printk("[mpu6050_probe] cdev_add failed\n");
        return -1;
    }
    printk("[mpu6050] driver client probed!\n");

    return 0;
}

static int mpu6050_remove(struct i2c_client *client){
    dev_t devno = MKDEV(major,minor);
    cdev_del(&pgmydev->mydev);
    unregister_chrdev_region(devno,mpu6050_num);
    kfree(pgmydev);

    return 0;
}

 static const struct i2c_device_id my_i2c_id[ ] = {
    {"mpu6050",0},
    {}
 };

 struct of_device_id mpu6050_dt[] = {
    {.compatible = "invensense,mpu6050"},
    {}
 };

struct i2c_driver mpu6050_driver = {
    .driver = {
        .name = "mpu6050",
        .owner = THIS_MODULE,
        .of_match_table = mpu6050_dt,
    },
    .probe = mpu6050_probe,
    .remove = mpu6050_remove,
    .id_table = my_i2c_id,
};

// module_i2c_driver(mpu6050_driver);

int __init mpu6050_driver_init(void){

    printk("[mpu6050] driver init\n");
    i2c_add_driver(&mpu6050_driver);
    return 0;
}
void __exit mpu6050_driver_exit(void){
    printk("[mpu6050] driver exit\n");
    i2c_del_driver(&mpu6050_driver);

}

module_init(mpu6050_driver_init);
module_exit(mpu6050_driver_exit);

MODULE_LICENSE("GPL");
