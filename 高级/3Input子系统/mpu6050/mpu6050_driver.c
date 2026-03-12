#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>


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


struct mpu6050_dev{

    struct input_dev *pinput;
    struct i2c_client *pclt;

    struct delayed_work work;
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


void mpu6050_work_func(struct work_struct *pwk){

    struct mpu6050_dev *pmydev = container_of((struct delayed_work *)pwk,struct mpu6050_dev,work);
 
    short ax = 0;
    short ay = 0;
    short az = 0;

    short gx = 0;
    short gy = 0;
    short gz = 0;

    short temp = 0;

    ax = (mpu6050_read_byte(pmydev->pclt,ACCEL_XOUT_H) << 8) | mpu6050_read_byte(pmydev->pclt,ACCEL_XOUT_L) ;
    input_report_abs(pmydev->pinput,ABS_X,ax);
    ay = (mpu6050_read_byte(pmydev->pclt,ACCEL_YOUT_H) << 8) | mpu6050_read_byte(pmydev->pclt,ACCEL_YOUT_L) ;
    input_report_abs(pmydev->pinput,ABS_Y,ay);
    az = (mpu6050_read_byte(pmydev->pclt,ACCEL_ZOUT_H) << 8) | mpu6050_read_byte(pmydev->pclt,ACCEL_ZOUT_L) ;
    input_report_abs(pmydev->pinput,ABS_Z,az);

    gx = mpu6050_read_byte(pmydev->pclt,GYRO_XOUT_L) | (mpu6050_read_byte(pmydev->pclt,GYRO_XOUT_H) << 8);
    input_report_abs(pmydev->pinput,ABS_RX,gx);
    gy = mpu6050_read_byte(pmydev->pclt,GYRO_YOUT_L) | (mpu6050_read_byte(pmydev->pclt,GYRO_YOUT_H) << 8);
    input_report_abs(pmydev->pinput,ABS_RY,gy);
    gz = mpu6050_read_byte(pmydev->pclt,GYRO_ZOUT_L) | (mpu6050_read_byte(pmydev->pclt,GYRO_ZOUT_H) << 8);
    input_report_abs(pmydev->pinput,ABS_RZ,gz);

    temp = mpu6050_read_byte(pmydev->pclt,TEMP_OUT_L) | (mpu6050_read_byte(pmydev->pclt,TEMP_OUT_H) << 8);
    input_report_abs(pmydev->pinput,ABS_MISC,temp);

    input_sync(pmydev -> pinput);

    schedule_delayed_work(&pgmydev->work, msecs_to_jiffies(1000));

}

void init_mpu6050(struct i2c_client *pclt){

    mpu6050_write_byte(pclt,PWR_MGMT_1,0x00);
    mpu6050_write_byte(pclt,SMPLRT_DIV,0x07);
    mpu6050_write_byte(pclt,CONFIG,0x06);
    mpu6050_write_byte(pclt,GYRO_CONFIG,0x00);
    mpu6050_write_byte(pclt,ACCEL_CONFIG,0x00);
}



static int mpu6050_probe(struct i2c_client* pclt,const struct i2c_device_id *pid){
    int ret = 0;
    
    pgmydev = (struct mpu6050_dev *)kmalloc(sizeof(struct mpu6050_dev),GFP_KERNEL);
    if(NULL == pgmydev){
        printk("[mpu6050_probe] kmalloc for pgmyedv failed\n");
        return -1;
    }
    memset(pgmydev,0,sizeof(struct mpu6050_dev));

    pgmydev -> pclt = pclt;

    init_mpu6050(pgmydev -> pclt);

    /**/
    pgmydev->pinput = input_allocate_device();

    set_bit(EV_ABS, pgmydev->pinput->evbit);
    input_set_abs_params(pgmydev->pinput,ABS_X,-32768,32768,0,0);
    input_set_abs_params(pgmydev->pinput,ABS_Y,-32768,32768,0,0);
    input_set_abs_params(pgmydev->pinput,ABS_Z,-32768,32768,0,0);

    input_set_abs_params(pgmydev->pinput,ABS_RX,-32768,32768,0,0);
    input_set_abs_params(pgmydev->pinput,ABS_RY,-32768,32768,0,0);
    input_set_abs_params(pgmydev->pinput,ABS_RZ,-32768,32768,0,0);

    input_set_abs_params(pgmydev->pinput,ABS_MISC,-32768,32768,0,0);

    /**/
    ret = input_register_device(pgmydev->pinput);
    if(ret){
        printk("input_register_device failed\n");

        input_free_device(pgmydev->pinput);
        kfree(pgmydev);
        pgmydev = NULL;
        return -1;
    }

    INIT_DELAYED_WORK(&pgmydev->work,mpu6050_work_func);

    schedule_delayed_work(&pgmydev->work, msecs_to_jiffies(1000));
    
    printk("[mpu6050] driver client probed!\n");

    return 0;
}

static int mpu6050_remove(struct i2c_client *client){

    cancel_delayed_work(&pgmydev->work);
    input_unregister_device(pgmydev->pinput);
    input_free_device(pgmydev->pinput);

    kfree(pgmydev);
    pgmydev = NULL;
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
