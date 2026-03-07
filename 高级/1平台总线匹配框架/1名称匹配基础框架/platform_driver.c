#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>



static int driver_probe(struct platform_device *dev){
    printk("platform: matck ok\n");
    return 0;
}

static int driver_remove(struct platform_device *dev){
    printk("platform: driver remove\n");
    return 0;
}

struct platform_driver test_driver = {
    .probe = driver_probe,
    .remove = driver_remove,
    .driver = {
        .name = "test_device",
    },
};

static int __init platform_driver_init(void){
    platform_driver_register(&test_driver);
    return 0;
}

static void __exit platform_driver_exit(void){
    platform_driver_unregister(&test_driver);
}

module_init(platform_driver_init);
module_exit(platform_driver_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("GHC");
MODULE_DESCRIPTION("平台总线框架学习代码");
