#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>



static void device_release(struct device *dev){
    printk("platfoem : device release\n");
}

struct platform_device test_device = {
    .id =1,
    .name = "test_device",
    .dev.release = device_release,
};

static int __init platform_device_init(void){
    platform_device_register(&test_device);
    return 0;
}

static void __exit platform_device_exit(void){
    platform_device_unregister(&test_device);

}

module_init(platform_device_init);
module_exit(platform_device_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("GHC");
MODULE_DESCRIPTION("平台总线框架学习代码");

