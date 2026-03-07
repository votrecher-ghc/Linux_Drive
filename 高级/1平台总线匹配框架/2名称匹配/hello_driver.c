#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>


int hello_driver_probe(struct platform_device* p_pltdev){
    struct resource *pres = NULL;

    /*取设备p_pltdev资源中IORESOURCE_MEM的第2个*/
    pres = platform_get_resource(p_pltdev,IORESOURCE_MEM,0);
    printk("name : %s res.start = 0x%x\n",pres->name,(unsigned int)pres->start);

    pres = platform_get_resource(p_pltdev,IORESOURCE_MEM,1);
    printk("name : %s res.start = 0x%x\n",pres->name,(unsigned int)pres->start);

    pres = platform_get_resource(p_pltdev,IORESOURCE_MEM,2);
    printk("name : %s res.start = 0x%x\n",pres->name,(unsigned int)pres->start);

    pres = platform_get_resource(p_pltdev,IORESOURCE_IRQ,0);
    printk("name : %s res.start = %d\n",pres->name,(int)pres->start);

    pres = platform_get_resource(p_pltdev,IORESOURCE_IRQ,1);
    printk("name : %s res.start = %d\n",pres->name,(int)pres->start);

    pres = platform_get_resource(p_pltdev,IORESOURCE_IRQ,2);
    printk("name : %s res.start = %d\n",pres->name,(int)pres->start);
    
    printk("hello_deiver_probe is called\n");
    return 0;
}

int hello_driver_remove(struct platform_device* p_pltdev){
      printk("hello_deiver_remove is called\n");
    return 0;
}

struct platform_driver hello_driver = {
    .driver.name = "hello",
    .probe = hello_driver_probe,
    .remove = hello_driver_remove,
};

static int __init hello_driver_init(void){

    platform_driver_register(&hello_driver);
    return 0;
}

static void __exit hello_driver_exit(void){
    platform_driver_unregister(&hello_driver);
}

module_init(hello_driver_init);
module_exit(hello_driver_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("GHC");
MODULE_DESCRIPTION("平台总线框架学习代码");
