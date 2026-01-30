#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>

#include "leddrv.h"

#define MAJOR_NUM 11
#define MINOR_NUM 0
#define DEV_NUM 1

int major = MAJOR_NUM;
int minor = MINOR_NUM;
int dev_num = DEV_NUM;


#define GPX1CON 0x11000c20
#define GPX1DAT 0x11000c24

#define GPX2CON 0x11000c40
#define GPX2DAT 0x11000c44

#define GPF3CON 0x114001E0
#define GPF3DAT 0x114001E4

struct myled_dev
{

    struct cdev mydev;

    volatile unsigned long *led2con;
    volatile unsigned long *led2dat;

    volatile unsigned long *led3con;
    volatile unsigned long *led3dat;

    volatile unsigned long *led4con;
    volatile unsigned long *led4dat;

    volatile unsigned long *led5con;
    volatile unsigned long *led5dat;
};

struct myled_dev *pgmyled = NULL;

int myled_open(struct inode *pnode, struct file *pfile)
{
    pfile->private_data = (void *)(container_of(pnode->i_cdev, struct myled_dev, mydev));
    return 0;
}

int myled_close(struct inode *pnode, struct file *pfile)
{

    return 0;
}

void led_on(struct myled_dev *pmydev,int ledno){
    switch(ledno){
        case 2:
            writel(readl(pmydev->led2dat) | (0x1 << 7),pmydev->led2dat);
            break;
        case 3:
            writel(readl(pmydev->led3dat) | (0x1),pmydev->led3dat);
            break;
        case 4:
            writel(readl(pmydev->led4dat) | (0x1 << 4),pmydev->led4dat);
            break;
        case 5:
            writel(readl(pmydev->led5dat) | (0x1 << 5),pmydev->led5dat);
            break;
    }
}
void led_off(struct myled_dev *pmydev,int ledno){

        switch(ledno){
        case 2:
            writel(readl(pmydev->led2dat) & (~(0x1 << 7)),pmydev->led2dat);
            break;
        case 3:
            writel(readl(pmydev->led3dat) & (~(0x1)),pmydev->led3dat);
            break;
        case 4:
            writel(readl(pmydev->led4dat) & (~(0x1 << 4)),pmydev->led4dat);
            break;
        case 5:
            writel(readl(pmydev->led5dat) & (~(0x1 << 5)),pmydev->led5dat);
            break;
    }

}

long myled_ioctl(struct file *pfile,unsigned int cmd,unsigned long arg){
    struct myled_dev *pmydev = (struct myled_dev *)pfile->private_data;

    if(arg < 2 || arg > 5){
        return -1;
    }

    switch(cmd){
        case MY_LED_OFF:
            led_off(pmydev,arg);
            break;
        case MY_LED_ON:
            led_on(pmydev,arg);
            break;
        default:
            printk("invalid cmd\n");
            break;
    }
    return 0;
}


void ioremap_ledreg(struct myled_dev * pmydev){

    pmydev->led2con = ioremap(GPX2CON,4);
    pmydev->led2dat = ioremap(GPX2DAT,4);

    pmydev->led3con = ioremap(GPX1CON,4);
    pmydev->led3dat = ioremap(GPX1DAT,4);

    pmydev->led4con = ioremap(GPF3CON,4);
    pmydev->led4dat = ioremap(GPF3DAT,4);

    pmydev->led5con = pmydev->led4con ;
    pmydev->led5dat = pmydev->led4dat ;      
}

void set_output_ledconreg(struct myled_dev * pmydev){
    /*设置led2con的28-31位为0001*/
    writel(((readl(pmydev->led2con) & (~(0xF << 28))) | (0x1 << 28)),pmydev->led2con);
    /*设置led3con的0-3位为0001*/
    writel(((readl(pmydev->led3con) & (~(0xF))) | (0x1)),pmydev->led3con);
    /*设置led4con的16-19位为0001*/
    writel(((readl(pmydev->led4con) & (~(0xF << 16))) | (0x1 << 16)),pmydev->led4con);
    /*设置led5con的20-23位为0001*/
    writel(((readl(pmydev->led5con) & (~(0xF << 20))) | (0x1 << 20)),pmydev->led5con);

    /*关闭led*/
    writel(readl(pmydev->led2dat) & (~(0x1 << 7)),pmydev->led2dat);
    writel(readl(pmydev->led3dat) & (~(0x1)),pmydev->led3dat);
    writel(readl(pmydev->led4dat) & (~(0x1 << 4)),pmydev->led4dat);
    writel(readl(pmydev->led5dat) & (~(0x1 << 5)),pmydev->led5dat);
}

void iounmap_ledreg(struct myled_dev * pmydev){

    iounmap(pmydev->led2con);
    pmydev->led2con = NULL;
    iounmap(pmydev->led2dat);
    pmydev->led2dat = NULL;

    iounmap(pmydev->led3con);
    pmydev->led3con = NULL;
    iounmap(pmydev->led3dat);
    pmydev->led3dat = NULL;

    iounmap(pmydev->led4con);
    pmydev->led4con = NULL;
    iounmap(pmydev->led4dat);
    pmydev->led4dat = NULL;

    pmydev->led5dat = NULL;
    pmydev->led5dat = NULL;

}

struct file_operations ops = {
    .owner = THIS_MODULE,
    .open = myled_open,
    .release = myled_close,
    .unlocked_ioctl = myled_ioctl,
};

int __init myled_init(void)
{

    int ret = 0;
    dev_t devno = MKDEV(major, minor);

    pgmyled = (struct myled_dev *)kmalloc(sizeof(struct myled_dev), GFP_KERNEL);
    if (NULL == pgmyled)
    {
        printk("failed to kmalloc for struct myled_dev\n");
        return -1;
    }

    ret = register_chrdev_region(devno, dev_num, "myled");
    if (ret)
    {
        ret = alloc_chrdev_region(&devno, minor, dev_num, "myled");
        if (ret)
        {
            printk("alloc devno failed\n");
            return -1;
        }
    }
    major = MAJOR(devno);
    minor = MINOR(devno);
    printk("get devno %d \n", major);

    cdev_init(&pgmyled->mydev, &ops);
    pgmyled->mydev.owner = THIS_MODULE;
    cdev_add(&pgmyled->mydev, devno, dev_num);

    /*ioremap*/
    ioremap_ledreg(pgmyled);
    /*set con-register output*/
    set_output_ledconreg(pgmyled);
    printk("myled load\n");
    return 0;
}

void __exit myled_exit(void)
{

    dev_t devno = MKDEV(major,minor);

    /*iounmap*/
    iounmap_ledreg(pgmyled);
    kfree(pgmyled);
    pgmyled = NULL;
    
    unregister_chrdev_region(devno,dev_num);
    printk("myldev exit\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GHC");
MODULE_DESCRIPTION("led驱动示例");

module_init(myled_init);
module_exit(myled_exit);
