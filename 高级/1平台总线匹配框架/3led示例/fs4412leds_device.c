#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/platform_device.h>

#define GPX1CON 0x11000c20
#define GPX1DAT 0x11000c24

#define GPX2CON 0x11000c40
#define GPX2DAT 0x11000c44

#define GPF3CON 0x114001E0
#define GPF3DAT 0x114001E4


void hello_dev_release(struct device *dev){
    printk("hello_dev_release is called\n");
}

/*资源信息*/
struct resource hello_dev_res[] = {
    [0] = {
        .start = GPX1CON,
        .end = GPX1CON + 3,
        .name = "gpx1con",
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = GPX1DAT,
        .end = GPX1DAT + 3,
        .name = "gpx1dat",
        .flags = IORESOURCE_MEM,
    },

    [2] = {
        .start = GPX2CON,
        .end = GPX2CON + 3,
        .name = "gpx2con",
        .flags = IORESOURCE_MEM,
    },
    [3] = {
        .start = GPX2DAT,
        .end = GPX2DAT + 3,
        .name = "gpx2dat",
        .flags = IORESOURCE_MEM,
    },

    [4] = {
        .start = GPF3CON,
        .end = GPF3CON + 3,
        .name = "gpf3con",
        .flags = IORESOURCE_MEM,
    },
    [5] = {
        .start = GPF3DAT,
        .end = GPF3DAT + 3,
        .name = "gpf3dat",
        .flags = IORESOURCE_MEM,
    },

};

struct platform_device hello_device = {
    .name = "fs4412leds",
    .dev.release = hello_dev_release,

    /*在设备内添加资源*/
    .resource = hello_dev_res,
    .num_resources = ARRAY_SIZE(hello_dev_res),
};

static int __init hello_device_init(void){
    platform_device_register(&hello_device);
    return 0;
}

static void __exit hello_device_exit(void){
    platform_device_unregister(&hello_device);
}

module_init(hello_device_init);
module_exit(hello_device_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("GHC");
MODULE_DESCRIPTION("平台总线框架学习代码");
