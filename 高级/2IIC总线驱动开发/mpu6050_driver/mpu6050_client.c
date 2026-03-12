#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/module.h>

static unsigned short mpu6050_addr_list[] = {
    0x68,
    0x69,
    I2C_CLIENT_END
};

static struct i2c_client *gpmpu6050_client = NULL;

static int __init mpu6050_dev_init(void){

    struct i2c_adapter *padp;
    static struct i2c_board_info mpu6050_info = {
        .type = "mpu6050",
    };

    padp = i2c_get_adapter(5);
    // gpmpu6050_client = i2c_new_device(padp,&mpu6050_info);
    gpmpu6050_client = i2c_new_probed_device(padp,&mpu6050_info,mpu6050_addr_list,NULL);
    i2c_put_adapter(padp);

    if(gpmpu6050_client != NULL){
        printk("[mpu6050] client init\n");
        return 0;
    }else{
        return -ENODEV;
    }

}

static void __exit mpu6050_dev_exit(void){
    i2c_unregister_device(gpmpu6050_client);
    printk("[mpu6050] client exit\n");

}

module_init(mpu6050_dev_init);
module_exit(mpu6050_dev_exit);

MODULE_LICENSE("GPL");
