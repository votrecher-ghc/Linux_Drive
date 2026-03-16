#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/of.h>
#include <linux/device.h>
#include <linux/mod_devicetable.h>



#include "fs4412_mpu6050.h"

#define MAJOR_NO 251
#define MINOR_NO 0
#define DEV_NUM 1

int major = MAJOR_NO;
int minor = MINOR_NO;
int dev_num = DEV_NUM;

/*mpu6050设备信息*/
struct fs4412_mpu6050_dev {
    struct cdev cdev;
    struct i2c_client *pclt;

    struct class *p_cls;
    struct device *p_dev;
};

/*全局设备结构体指针*/
struct fs4412_mpu6050_dev* pgfs4412_mpu6050_dev = NULL;

/*设备操作函数集---open*/
static int fs4412_mpu6050_open(struct inode *pnode, struct file *pfile){
    printk("mpu6050 open\n");
    pfile->private_data = (void *)(container_of(pnode->i_cdev,struct fs4412_mpu6050_dev,cdev));

    return 0;
}

/*设备操作函数集---release*/
static int fs4412_mpu6050_close(struct inode *pnode, struct file *pfile){
    printk("mpu6050 close\n");
    return 0;
}


static int mpu6050_write_byte(struct i2c_client *pclt,unsigned char reg,unsigned char val);
static int mpu6050_read_byte(struct i2c_client *pclt,unsigned char reg);

/*设备操作函数集---unlocked_ioctl；arg为用户空间接收数据的地址*/
static long fs4412_mpu6050_ioctl(struct file *pfile,unsigned cmd,unsigned long arg){
    struct fs4412_mpu6050_dev * pfs4412_mpu6050_dev = (struct fs4412_mpu6050_dev *)pfile->private_data;

    union mpu6050_data data;
    struct mpu6050_all_data all_data;

    void * src_ptr = NULL;
    size_t copy_size = 0;

    switch(cmd){
        case GET_ACCEL:
            data.accel.x = (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,ACCEL_XOUT_H) << 8)|
                           (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,ACCEL_XOUT_L));
            data.accel.y = (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,ACCEL_YOUT_H) << 8)|
                           (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,ACCEL_YOUT_L));
            data.accel.z = (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,ACCEL_ZOUT_H) << 8)|
                           (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,ACCEL_ZOUT_L));
            src_ptr = &data;
            copy_size = sizeof(data);
            break;
        case GET_GYRO:
            data.gyro.x =   (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,GYRO_XOUT_H) << 8)|
                            (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,GYRO_XOUT_L));
            data.gyro.y =   (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,GYRO_YOUT_H) << 8)|
                            (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,GYRO_YOUT_L));
            data.gyro.z =   (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,GYRO_ZOUT_H) << 8)|
                            (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,GYRO_ZOUT_L));
            src_ptr = &data;
            copy_size = sizeof(data);
            break;
        case GET_TEMP:
            data.temp.temp =    (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt, TEMP_OUT_H) << 8)|
                                (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,TEMP_OUT_L));
            src_ptr = &data;
            copy_size = sizeof(data);
            break;
        case GET_ALL:

            all_data.accel.x =  (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,ACCEL_XOUT_H) << 8)|
                                (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,ACCEL_XOUT_L));
            all_data.accel.y =  (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,ACCEL_YOUT_H) << 8)|
                                (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,ACCEL_YOUT_L));
            all_data.accel.z =  (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,ACCEL_ZOUT_H) << 8)|
                                (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,ACCEL_ZOUT_L));

            all_data.gyro.x =   (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,GYRO_XOUT_H) << 8)|
                                (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,GYRO_XOUT_L));
            all_data.gyro.y =   (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,GYRO_YOUT_H) << 8)|
                                (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,GYRO_YOUT_L));
            all_data.gyro.z =   (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,GYRO_ZOUT_H) << 8)|
                                (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,GYRO_ZOUT_L));

             all_data.temp.temp =   (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt, TEMP_OUT_H) << 8)|
                                    (mpu6050_read_byte(pfs4412_mpu6050_dev->pclt,TEMP_OUT_L));

            src_ptr = &all_data;
            copy_size = sizeof(all_data);
            break;
        default:
            printk("Invalid cmd\n");
            return -EINVAL;
    }

    if(src_ptr != NULL && copy_size > 0){
        if(copy_to_user((void __user*)arg,src_ptr,copy_size)){
            printk("[fs4412_mpu6050_ioctl] copy_to_user failed\n");
            return -EFAULT;            
        }
    }

    return 0;
}

/*mpu6050操作函数集*/
static const struct file_operations fs4412_mpu6050_ops = {
    .owner = THIS_MODULE,
    .open = fs4412_mpu6050_open,
    .release = fs4412_mpu6050_close,
    .unlocked_ioctl = fs4412_mpu6050_ioctl,
};

/*向mpu6050对应寄存器写数据*/
static int mpu6050_write_byte(struct i2c_client *pclt,unsigned char reg,unsigned char val){
    int ret = 0;
    unsigned char data_buf[2] = {reg,val};
    struct i2c_msg msg = {.addr = pclt->addr, .flags = 0, .len = 2, .buf = data_buf};
    ret = i2c_transfer(pclt->adapter,&msg,1);
    if(ret != 1){
        return ret;
    }
    return 0;
}

/*读取mpu6050对应寄存器的数据*/
static int mpu6050_read_byte(struct i2c_client *pclt,unsigned char reg){
    int ret = 0;
    //无符号的 unsigned char，就是告诉编译器：“不要管它是正是负，这就是纯粹的 8 个物理比特位”。
    //这样把高低八位干净利落地拼接成 16 位之后，再把它交由有符号的类型去解析，正负值就完美还原了。
    unsigned char txbuf[1] = {reg};
    unsigned char rxbuf[1] = {0};
    struct i2c_msg msg[2] = {
        {pclt->addr, 0, 1, txbuf},
        {pclt->addr, I2C_M_RD, 1, rxbuf},
    };

    ret = i2c_transfer(pclt->adapter,msg,ARRAY_SIZE(msg));
    if(ret != ARRAY_SIZE(msg)){
        return -EINVAL;
    }
    return rxbuf[0];
}

/*初始化mpu6050*/
static int init_fs4412_mpu6050(struct i2c_client *pclt){
    if( mpu6050_write_byte(pclt,PWR_MGMT_1,0x00)     ||
        mpu6050_write_byte(pclt,SMPLRT_DIV,0x07)     ||
        mpu6050_write_byte(pclt,CONFIG,0x06)         ||
        mpu6050_write_byte(pclt,GYRO_CONFIG,0x00)    ||
        mpu6050_write_byte(pclt,ACCEL_CONFIG,0x00)   ){
        return -EINVAL;
        }
    return 0;
}


/*iic匹配函数*/
static int fs4412_mpu6050_probe(struct i2c_client *pclt,const struct i2c_device_id *pid){

    int ret = 0;
    dev_t devno = MKDEV(major,minor);

    /*为全局设备开辟空间*/
    pgfs4412_mpu6050_dev = (struct fs4412_mpu6050_dev *)kmalloc(sizeof(struct fs4412_mpu6050_dev),GFP_KERNEL);
    if(NULL == pgfs4412_mpu6050_dev){
        printk("[fs4412_mpu6050_probe] kmalloc failed\n");
        return -EFAULT;
    }
    memset(pgfs4412_mpu6050_dev,0,sizeof(struct fs4412_mpu6050_dev));

    /*申请设备号*/
    ret = register_chrdev_region(devno,dev_num,"fs4412_mpu6050");
    if(ret){
        ret = alloc_chrdev_region(&devno,minor,dev_num,"fs4412-mpu6050");
        if(ret){
            printk("[fs4412_mpu6050_probe] alloc_chrdev_region failed\n");
            kfree(pgfs4412_mpu6050_dev);
            pgfs4412_mpu6050_dev = NULL;
            return -EINVAL;
        }
        major = MAJOR(devno);
        minor = MINOR(devno);
    }
    printk("get major devno: %d,minor devno: %d\n",major,minor);

    /*初始化字符设备*/
    cdev_init(&pgfs4412_mpu6050_dev->cdev,&fs4412_mpu6050_ops);
    pgfs4412_mpu6050_dev -> cdev.owner = THIS_MODULE;

    /*将设备添加到内核*/
    ret = cdev_add(&pgfs4412_mpu6050_dev->cdev,devno,dev_num);
    if(ret){
        printk("[fs4412_mpu6050_probe] cdev_add failed\n");
        unregister_chrdev_region(devno,dev_num);
        kfree(pgfs4412_mpu6050_dev);
        pgfs4412_mpu6050_dev = NULL;
        return -EINVAL;
    }

    /*初始化mpu6050*/
    pgfs4412_mpu6050_dev -> pclt = pclt;
    if(init_fs4412_mpu6050(pgfs4412_mpu6050_dev -> pclt)){
        printk("[fs4412_mpu6050_probe] init_fs4412_mpu6050 failed\n");

        cdev_del(&pgfs4412_mpu6050_dev->cdev);
        unregister_chrdev_region(devno,dev_num);

        kfree(pgfs4412_mpu6050_dev);
        pgfs4412_mpu6050_dev = NULL;
        return -EINVAL;
    }

    /*自动mknod*/
    pgfs4412_mpu6050_dev->p_cls = class_create(THIS_MODULE,"fs4412-mpu6050");
    if(IS_ERR(pgfs4412_mpu6050_dev->p_cls)){
        printk("[fs4412_mpu6050_probe] class_create failed\n");

        cdev_del(&pgfs4412_mpu6050_dev->cdev);
        unregister_chrdev_region(devno,dev_num);

        kfree(pgfs4412_mpu6050_dev);
        pgfs4412_mpu6050_dev = NULL;
        return -EINVAL;

    }
    pgfs4412_mpu6050_dev->p_dev = device_create(pgfs4412_mpu6050_dev->p_cls,NULL,devno,NULL,"fs4412-mpu6050");
    if(IS_ERR(pgfs4412_mpu6050_dev->p_dev)){
        printk("[fs4412_mpu6050_probe] device_create failed\n");

        class_destroy(pgfs4412_mpu6050_dev->p_cls);
        pgfs4412_mpu6050_dev->p_cls =NULL;

        cdev_del(&pgfs4412_mpu6050_dev->cdev);
        unregister_chrdev_region(devno,dev_num);

        kfree(pgfs4412_mpu6050_dev);
        pgfs4412_mpu6050_dev = NULL;
        return -EINVAL;
    }
    return 0;
}

/*iic移除函数*/
static int fs4412_mpu6050_remove(struct i2c_client *pclt){

    dev_t devno = MKDEV(major,minor);

    if(pgfs4412_mpu6050_dev){

        if(pgfs4412_mpu6050_dev ->p_dev){
            device_destroy(pgfs4412_mpu6050_dev->p_cls,devno);
            pgfs4412_mpu6050_dev->p_dev = NULL;
        }
        if(pgfs4412_mpu6050_dev -> p_cls){
            class_destroy(pgfs4412_mpu6050_dev->p_cls);
            pgfs4412_mpu6050_dev->p_cls = NULL;
        }

        cdev_del(&pgfs4412_mpu6050_dev->cdev);
        unregister_chrdev_region(devno,dev_num);

        kfree(pgfs4412_mpu6050_dev);
        pgfs4412_mpu6050_dev = NULL;
        return 0;
    }

    return -EFAULT;
}

/*i2c名称列表*/
static const struct i2c_device_id fs4412_mpu6050_id[] = {
    {"mpu6050",0},
    {}
};
MODULE_DEVICE_TABLE(i2c,fs4412_mpu6050_id);

/*设备树名称列表*/
static const struct of_device_id fs4412_mpu6050_dt[] = {
    {.compatible = "invensense,mpu6050"},
    {}
};
MODULE_DEVICE_TABLE(of,fs4412_mpu6050_dt);

struct i2c_driver fs4412_mpu6050_driver = {
    .driver = {
        .name = "fs4412_mpu6050",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(fs4412_mpu6050_dt),
    },
    .probe = fs4412_mpu6050_probe,
    .remove = fs4412_mpu6050_remove,
    .id_table = fs4412_mpu6050_id,
};

#if 0
module_i2c_driver(fs4412_mpu6050_driver);
#else

int __init fs4412_mpu6050_driver_init(void){
    i2c_add_driver(&fs4412_mpu6050_driver);
    return 0;
}
void __exit fs4412_mpu6050_driver_exit(void){
    i2c_del_driver(&fs4412_mpu6050_driver);
}

module_init(fs4412_mpu6050_driver_init);
module_exit(fs4412_mpu6050_driver_exit);
#endif

MODULE_LICENSE("GPL");
