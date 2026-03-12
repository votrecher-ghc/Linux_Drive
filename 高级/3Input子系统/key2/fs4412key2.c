#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

struct fs4412key2_dev{

    struct input_dev* pdev;
    int gpio;
    int irqno;

};

struct fs4412key2_dev* pgmydev = NULL;


irqreturn_t key2_irq_handle(int no,void* arg){
    struct fs4412key2_dev *pmydev = (struct fs4412key2_dev *)arg;
    int status1;
    int status2;
    
    status1 = gpio_get_value(pmydev->gpio);
    mdelay(1);
    status2 = gpio_get_value(pmydev->gpio);

    if(status1 != status2){
        return IRQ_NONE;
    }
    if(status1){
        input_event(pgmydev->pdev,EV_KEY,KEY_2,0);
        input_sync(pgmydev->pdev);
    }else{
        input_event(pgmydev->pdev,EV_KEY,KEY_2,1);
        input_sync(pgmydev->pdev);
    }

    return IRQ_HANDLED;

}


int __init fs4412key2_init(void){

    int ret  = 0;
  
    struct device_node *pnode = NULL;

    /*在内核中为设备分配空间*/
    pgmydev = (struct fs4412key2_dev*)kmalloc(sizeof(struct fs4412key2_dev),GFP_KERNEL);
    if(NULL == pgmydev){
      
        printk("kmalloc failed\n");
        return -1;
    }
    memset(pgmydev,0,sizeof(struct fs4412key2_dev));

    /*找到设备树中led节点*/
    pnode = of_find_node_by_path("/mykey2_node");
    if(NULL == pnode)
    {
        printk("of_find_node_by_path failed\n");
        return -1;
    }
    
    /*找到设备树中的giop节点*/
    pgmydev->gpio = of_get_named_gpio(pnode,"key2-gpio",0);
    pgmydev->irqno = irq_of_parse_and_map(pnode,0);

    /*初始化input_dev*/
    pgmydev->pdev = input_allocate_device();
        /*设置外设处理EV_KEY事件*/
    set_bit(EV_KEY,pgmydev->pdev->evbit);
        /*设置上报的按键*/
    set_bit(KEY_2,pgmydev->pdev->keybit);

    /*注册input_dev*/
    ret = input_register_device(pgmydev->pdev);
    if(ret){
        kfree(pgmydev);
        pgmydev = NULL;
        printk("input_register_device failed\n");
        return -1;
    }

    /*设置中断*/
    ret = request_irq(pgmydev->irqno,key2_irq_handle,IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,"fs4412key2",pgmydev);
    if(ret){

        input_unregister_device(pgmydev->pdev);
        input_free_device(pgmydev->pdev);
        kfree(pgmydev);
        pgmydev = NULL;
        printk("request_irq failed\n");
        return -1;
    }

    return 0;
}

void __exit fs4412key2_exit(void){
  
    input_unregister_device(pgmydev->pdev);
    input_free_device(pgmydev->pdev);
    free_irq(pgmydev->irqno,pgmydev);

    kfree(pgmydev);
    pgmydev = NULL;
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("GHC");
MODULE_DESCRIPTION("中断练习代码");

module_init(fs4412key2_init);
module_exit(fs4412key2_exit);
