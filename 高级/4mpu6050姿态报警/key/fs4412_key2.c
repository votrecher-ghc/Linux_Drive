#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <asm/uaccess.h>


struct fs4412_key_dev{

    //必须用指针
    struct input_dev * pidev;
    int gpio_key2;
    int gpio_key3;
    int gpio_key4;

    int irqno_key2;
    int irqno_key3;
    int irqno_key4;
};
struct fs4412_key_dev *pgfs4412_key_dev = NULL;


irqreturn_t key2_irq_handle(int irq ,void *arg){
    struct fs4412_key_dev *pfs4412_key_dev = (struct fs4412_key_dev *)arg;
    int status1;
    int status2;
    status1 = gpio_get_value(pfs4412_key_dev -> gpio_key2);
    mdelay(1);
    status2 = gpio_get_value(pfs4412_key_dev -> gpio_key2);
    if(status1 != status2){
        return IRQ_NONE;
    }
    if(status1){
        input_event(pfs4412_key_dev -> pidev,EV_KEY,KEY_2,0);
        input_sync(pfs4412_key_dev -> pidev);
    }else{
        input_event(pfs4412_key_dev -> pidev,EV_KEY,KEY_2,1);
        input_sync(pfs4412_key_dev -> pidev);       
    }

    return IRQ_HANDLED;

}

irqreturn_t key3_irq_handle(int irq ,void *arg){
    struct fs4412_key_dev *pfs4412_key_dev = (struct fs4412_key_dev *)arg;
    int status1;
    int status2;
    status1 = gpio_get_value(pfs4412_key_dev -> gpio_key3);
    mdelay(1);
    status2 = gpio_get_value(pfs4412_key_dev -> gpio_key3);
    if(status1 != status2){
        return IRQ_NONE;
    }
    if(status1){
        input_event(pfs4412_key_dev -> pidev,EV_KEY,KEY_3,0);
        input_sync(pfs4412_key_dev -> pidev);
    }else{
        input_event(pfs4412_key_dev -> pidev,EV_KEY,KEY_3,1);
        input_sync(pfs4412_key_dev -> pidev);       
    }

    return IRQ_HANDLED;

}


irqreturn_t key4_irq_handle(int irq ,void *arg){
    struct fs4412_key_dev *pfs4412_key_dev = (struct fs4412_key_dev *)arg;
    int status1;
    int status2;
    status1 = gpio_get_value(pfs4412_key_dev -> gpio_key4);
    mdelay(1);
    status2 = gpio_get_value(pfs4412_key_dev -> gpio_key4);
    if(status1 != status2){
        return IRQ_NONE;
    }
    if(status1){
        input_event(pfs4412_key_dev -> pidev,EV_KEY,KEY_4,0);
        input_sync(pfs4412_key_dev -> pidev);
    }else{
        input_event(pfs4412_key_dev -> pidev,EV_KEY,KEY_4,1);
        input_sync(pfs4412_key_dev -> pidev);       
    }

    return IRQ_HANDLED;

}

/*初始化按键*/
int key2_init(struct fs4412_key_dev *pgfs4412_key_dev){

    /*按键的设备树节点*/
    struct device_node *pnode = NULL;

    /*查找设备树节点key2*/
    pnode = of_find_node_by_path("/fs4412-key/key2_node");
    if(pnode == NULL){
        printk("[key2_init] of_find_node_by_path failed\n");
        return -EFAULT;
    }
    /*申请占用gpio与irq key2*/
    pgfs4412_key_dev -> gpio_key2 = of_get_named_gpio(pnode,"key2-gpio",0);
    pgfs4412_key_dev -> irqno_key2 = irq_of_parse_and_map(pnode,0);
    if(pgfs4412_key_dev -> gpio_key2 < 0 || pgfs4412_key_dev -> irqno_key2 == 0){
        printk("[key_init] of_get_named_gpio or irq_of_parse_and_map failed\n");
        return -EFAULT;
    }

    /*查找设备树节点key3*/
    pnode = of_find_node_by_path("/fs4412-key/key3_node");
    if(pnode == NULL){
        printk("[key_init] of_find_node_by_path failed\n");
        return -EFAULT;
    }
    /*申请占用gpio与irq key3*/
    pgfs4412_key_dev -> gpio_key3 = of_get_named_gpio(pnode,"key3-gpio",0);
    pgfs4412_key_dev -> irqno_key3 = irq_of_parse_and_map(pnode,0);
    if(pgfs4412_key_dev -> gpio_key3 < 0 || pgfs4412_key_dev -> irqno_key3 == 0){
        printk("[key_init] of_get_named_gpio or irq_of_parse_and_map failed\n");
        return -EFAULT;
    }

    /*查找设备树节点key4*/
    pnode = of_find_node_by_path("/fs4412-key/key4_node");
    if(pnode == NULL){
        printk("[key_init] of_find_node_by_path failed\n");
        return -EFAULT;
    }
    /*申请占用gpio与irq key4*/
    pgfs4412_key_dev -> gpio_key4 = of_get_named_gpio(pnode,"key4-gpio",0);
    pgfs4412_key_dev -> irqno_key4 = irq_of_parse_and_map(pnode,0);
    if(pgfs4412_key_dev -> gpio_key4 < 0 || pgfs4412_key_dev -> irqno_key4 == 0){
        printk("[key_init] of_get_named_gpio or irq_of_parse_and_map failed\n");
        return -EFAULT;
    }

    return 0;
}

int __init fs4412_key_init(void){
    int ret = 0;

    /*在内核中为设备分配空间*/
    pgfs4412_key_dev = (struct fs4412_key_dev *)kmalloc(sizeof(struct fs4412_key_dev),GFP_KERNEL);
    if(pgfs4412_key_dev == NULL){
        printk("[fs4412_key2_init] kmalloc failed\n");
        return -EFAULT;
    }
    memset(pgfs4412_key_dev,0,sizeof(struct fs4412_key_dev));

    /*初始化key2*/
    if(key2_init(pgfs4412_key_dev)){
        printk("[fs4412_key2_init] key2_init failed\n");
        kfree(pgfs4412_key_dev);
        pgfs4412_key_dev = NULL;
        return -EINVAL;
    }

    /**
     * cdev (底层构件)： 允许自己定义实体，用 cdev_init() 手动装配。
     * 
     * input_dev (高级子系统)： 强制由内核包办，
     * 必须用指针接收 input_allocate_device() 的返回值。
     * 如果不用完或者出错退出了，还得用 input_free_device() 把它还给内核。
     */
    pgfs4412_key_dev -> pidev = input_allocate_device();
    if(pgfs4412_key_dev -> pidev == NULL){
        printk("[fs4412_key2_init] input_allocate_device failed\n");
        kfree(pgfs4412_key_dev);
        pgfs4412_key_dev = NULL;
        return -EINVAL;
    }
        /*设置外设处理EV_KEY事件*/
    set_bit(EV_KEY,pgfs4412_key_dev -> pidev-> evbit);
        /*设置上报的按键*/
    set_bit(KEY_2,pgfs4412_key_dev -> pidev ->keybit);
    set_bit(KEY_3,pgfs4412_key_dev -> pidev ->keybit);
    set_bit(KEY_4,pgfs4412_key_dev -> pidev ->keybit);

    /*注册inpu_tdev*/
    ret = input_register_device(pgfs4412_key_dev -> pidev);
    if(ret){
        printk("[fs4412_key2_init] input_register_device failed\n");
        input_free_device(pgfs4412_key_dev -> pidev);
        kfree(pgfs4412_key_dev);
        pgfs4412_key_dev = NULL;
        return -EINVAL;
    }

    /*设置中断:参数：中断号，中断处理函数，触发标志，中断名称,传递给中断处理函数 handler 的私有上下文数据*/
    //key2
    ret = request_irq(pgfs4412_key_dev -> irqno_key2,key2_irq_handle,IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,"fs4412-key2",pgfs4412_key_dev);
    if(ret){
        printk("[fs4412_key2_init] request_irq failed\n");
        input_unregister_device(pgfs4412_key_dev -> pidev);
        kfree(pgfs4412_key_dev);
        pgfs4412_key_dev = NULL;
        return -EINVAL;
    }

    //key3
    ret = request_irq(pgfs4412_key_dev -> irqno_key3,key3_irq_handle,IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,"fs4412-key3",pgfs4412_key_dev);
    if(ret){
        printk("[fs4412_key2_init] request_irq failed\n");
        free_irq(pgfs4412_key_dev->irqno_key2,pgfs4412_key_dev);  
        input_unregister_device(pgfs4412_key_dev -> pidev);
        kfree(pgfs4412_key_dev);
        pgfs4412_key_dev = NULL;
        return -EINVAL;
    }

    //key4
    ret = request_irq(pgfs4412_key_dev -> irqno_key4,key4_irq_handle,IRQF_TRIGGER_FALLING|IRQF_TRIGGER_RISING,"fs4412-key4",pgfs4412_key_dev);
    if(ret){
        printk("[fs4412_key2_init] request_irq failed\n");
        free_irq(pgfs4412_key_dev->irqno_key2,pgfs4412_key_dev);        
        free_irq(pgfs4412_key_dev->irqno_key3,pgfs4412_key_dev);  
        input_unregister_device(pgfs4412_key_dev -> pidev);
        kfree(pgfs4412_key_dev);
        pgfs4412_key_dev = NULL;
        return -EINVAL;
    }

    return 0;
}

void __exit fs4412_key_exit(void){
    if(pgfs4412_key_dev != NULL){

        if(pgfs4412_key_dev -> irqno_key2){
            free_irq(pgfs4412_key_dev->irqno_key2,pgfs4412_key_dev);        
        }
        if(pgfs4412_key_dev -> irqno_key3){
            free_irq(pgfs4412_key_dev->irqno_key3,pgfs4412_key_dev);        
        }
        if(pgfs4412_key_dev -> irqno_key4){
            free_irq(pgfs4412_key_dev->irqno_key4,pgfs4412_key_dev);        
        }

        input_unregister_device(pgfs4412_key_dev -> pidev);
        // input_free_device(pgfs4412_key_dev -> pidev);

        kfree(pgfs4412_key_dev);
        pgfs4412_key_dev = NULL;
    }
}

module_init(fs4412_key_init);
module_exit(fs4412_key_exit);

MODULE_LICENSE("GPL");