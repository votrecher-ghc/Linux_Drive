#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/of.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/device.h>
#include "fs4412_leds.h"


/**
 * LED驱动直接配置寄存器版本；
 * 通过ioctl控制led量灭
 */


/*设备号相关*/
#define MAJOR_NO 250
#define MINOR_NO 0
#define DEV_NUM 1

int major = MAJOR_NO;
int minor = MINOR_NO;
int dev_num = DEV_NUM;


/*led设备*/
struct fs4412_leds {
    struct cdev cdev;

    /*led2-5的控制寄存器以及数据寄存器*/
    void __iomem *led2con;
    void __iomem *led2dat;
    unsigned int led2bit;

    void __iomem *led3con;
    void __iomem *led3dat;
    unsigned int led3bit;

    void __iomem *led4con;
    void __iomem *led4dat;
    unsigned int led4bit;

    void __iomem *led5con;
    void __iomem *led5dat;
    unsigned int led5bit;

    struct class *pcls;
    struct device *pdev;
};

/*全局设备指针*/
struct fs4412_leds *pgfs4412_leds = NULL;

/*设备打开函数*/
static int fs4412_leds_open(struct inode *pnode,struct file *pfile){

    printk("fs4412-leds open\n");

    /*将设备信息保存到pfile中*/
    pfile->private_data = (struct fs4412_leds *)container_of(pnode->i_cdev,struct fs4412_leds,cdev);
    return 0;
}

/*设备关闭函数*/
static int fs4412_leds_close(struct inode *pnode,struct file *pfile){
    printk("fs4412-leds close\n");

    return 0;
}

/*开启指定号数led*/
static int led_on(struct fs4412_leds * pfs4412_leds,unsigned char ledno){

    //保证只操作对应的dat位，先读取对应寄存器的值，在将对应位或上1，即可将对应位置1
    switch(ledno){
        case 2:
            writel(readl(pfs4412_leds -> led2dat) | (1 << pfs4412_leds->led2bit),pfs4412_leds -> led2dat);
            break;
        case 3:
            writel(readl(pfs4412_leds -> led3dat) | (1 << pfs4412_leds->led3bit),pfs4412_leds -> led3dat);
            break;
        case 4:
            writel(readl(pfs4412_leds -> led4dat) | (1 << pfs4412_leds->led4bit),pfs4412_leds -> led4dat);
            break;
        case 5:
            writel(readl(pfs4412_leds -> led5dat) | (1 << pfs4412_leds->led5bit),pfs4412_leds -> led5dat);
            break;
        default:
            printk("[led_on] Invalid ledno\n");
            return -EINVAL;
    }
    return 0;
}

/*关闭指定号数led*/
static int led_off(struct fs4412_leds * pfs4412_leds,unsigned char ledno){

    //保证只操作对应的dat位，先读取对应寄存器的值，在将对应位与上0，即可将对应位置0
    switch(ledno){
        case 2:
            writel(readl(pfs4412_leds -> led2dat) & (~(1 << pfs4412_leds->led2bit)),pfs4412_leds -> led2dat);
            break;
        case 3:
            writel(readl(pfs4412_leds -> led3dat) & (~(1 << pfs4412_leds->led3bit)),pfs4412_leds -> led3dat);
            break;
        case 4:
            writel(readl(pfs4412_leds -> led4dat) & (~(1 << pfs4412_leds->led4bit)),pfs4412_leds -> led4dat);
            break;
        case 5:
            writel(readl(pfs4412_leds -> led5dat) & (~(1 << pfs4412_leds->led5bit)),pfs4412_leds -> led5dat);
            break;
        default:
            printk("[led_off] Invalid ledno\n");
            return -EINVAL;
    }

    return 0;
}

/*ioctl驱动函数*/
static long fs4412_leds_ioctl(struct file *pfile,unsigned int cmd,unsigned long arg){

    struct fs4412_leds * pfs4412_leds = (struct fs4412_leds *)pfile->private_data;

    switch(cmd){
        case LED_ON:
            return (led_on(pfs4412_leds,(unsigned char)arg));
        case LED_OFF:
            return(led_off(pfs4412_leds,(unsigned char)arg));
        default:
            printk("Invalid cmd\n");
            return -EINVAL;
    }
    return 0;
}

/*驱动操作函数集*/
static const struct file_operations fs4412_leds_ops = {
    .owner = THIS_MODULE,
    .open = fs4412_leds_open,
    .release = fs4412_leds_close,
    .unlocked_ioctl = fs4412_leds_ioctl,
};


/*led地址解除映射*/
void iounmap_leds(struct fs4412_leds *pfs4412_leds){
    if(pfs4412_leds->led2con) {iounmap(pfs4412_leds->led2con);pfs4412_leds->led2con = NULL;}
    if(pfs4412_leds->led2dat) {iounmap(pfs4412_leds->led2dat);pfs4412_leds->led2dat = NULL;}

    if(pfs4412_leds->led3con) {iounmap(pfs4412_leds->led3con);pfs4412_leds->led3con = NULL;}
    if(pfs4412_leds->led3dat) {iounmap(pfs4412_leds->led3dat);pfs4412_leds->led3dat = NULL;}

    if(pfs4412_leds->led4con) {iounmap(pfs4412_leds->led4con);pfs4412_leds->led4con = NULL;}
    if(pfs4412_leds->led4dat) {iounmap(pfs4412_leds->led4dat);pfs4412_leds->led4dat = NULL;}

    if(pfs4412_leds->led5con) {iounmap(pfs4412_leds->led5con);pfs4412_leds->led5con = NULL;}
    if(pfs4412_leds->led5dat) {iounmap(pfs4412_leds->led5dat);pfs4412_leds->led5dat = NULL;}
}

/*led地址映射*/
int ioremap_leds(struct platform_device *pdev,struct fs4412_leds *pfs4412_leds){

    struct device_node *pnode = NULL;
    struct device_node *pchild_node = NULL;
    unsigned int con, dat, bit;

    //设备树中的属性信息保存在pdev中的dev中
    pnode = pdev->dev.of_node;
    if(NULL == pnode){
        printk("[ioremap_leds] pnode is NULL\n");
        return -EINVAL;
    }
    
    //拿到子结点led2
    pchild_node = of_get_child_by_name(pnode,"led2");
    if(!pchild_node){
        printk("[ioremap_leds] of_get_child_by_name failed\n");
        return -EINVAL;
    }
    //拿到子结点属性值
    if( of_property_read_u32(pchild_node,"con-reg",&con) ||
        of_property_read_u32(pchild_node,"dat-reg",&dat) ||
        of_property_read_u32(pchild_node,"bit",&bit)){
            of_node_put(pchild_node);
            printk("[ioremap_leds] of_property_read_u32 failed\n");
            return -EINVAL;
    }
    pfs4412_leds -> led2con = ioremap(con,4);
    pfs4412_leds -> led2dat = ioremap(dat,4);
    pfs4412_leds -> led2bit = bit;
    of_node_put(pchild_node);
    if(!pfs4412_leds -> led2con || !pfs4412_leds -> led2dat){
        printk("[ioremap_leds] ioremap failed\n");
        return -EINVAL;
    }

    //拿到子结点led3
    pchild_node = of_get_child_by_name(pnode,"led3");
    if(!pchild_node){
        printk("[ioremap_leds] of_get_child_by_name failed\n");
        return -EINVAL;
    }
    //拿到子结点属性值
    if( of_property_read_u32(pchild_node,"con-reg",&con) ||
        of_property_read_u32(pchild_node,"dat-reg",&dat) ||
        of_property_read_u32(pchild_node,"bit",&bit)){
            printk("[ioremap_leds] of_property_read_u32 failed\n");
            of_node_put(pchild_node);
            iounmap_leds(pfs4412_leds);
            return -EINVAL;
    }
    pfs4412_leds -> led3con = ioremap(con,4);
    pfs4412_leds -> led3dat = ioremap(dat,4);
    pfs4412_leds -> led3bit = bit;
    of_node_put(pchild_node);
    if(!pfs4412_leds -> led3con || !pfs4412_leds -> led3dat){
        printk("[ioremap_leds] ioremap failed\n");
        return -EINVAL;
    }

    //拿到子结点led4
    pchild_node = of_get_child_by_name(pnode,"led4");
    if(!pchild_node){
        printk("[ioremap_leds] of_get_child_by_name failed\n");
        return -EINVAL;
    }
    //拿到子结点属性值
    if( of_property_read_u32(pchild_node,"con-reg",&con) ||
        of_property_read_u32(pchild_node,"dat-reg",&dat) ||
        of_property_read_u32(pchild_node,"bit",&bit)){
            of_node_put(pchild_node);
            iounmap_leds(pfs4412_leds);
            printk("[ioremap_leds] of_property_read_u32 failed\n");
            return -EINVAL;
    }
    pfs4412_leds -> led4con = ioremap(con,4);
    pfs4412_leds -> led4dat = ioremap(dat,4);
    pfs4412_leds -> led4bit = bit;
    of_node_put(pchild_node);
    if(!pfs4412_leds -> led4con || !pfs4412_leds -> led4dat){
        printk("[ioremap_leds] ioremap failed\n");
        return -EINVAL;
    }
    

    //拿到子结点led5
    pchild_node = of_get_child_by_name(pnode,"led5");
    if(!pchild_node){
        printk("[ioremap_leds] of_get_child_by_name failed\n");
        return -EINVAL;
    }
    //拿到子结点属性值
    if( of_property_read_u32(pchild_node,"con-reg",&con) ||
        of_property_read_u32(pchild_node,"dat-reg",&dat) ||
        of_property_read_u32(pchild_node,"bit",&bit)){
            of_node_put(pchild_node);
            iounmap_leds(pfs4412_leds);
            printk("[ioremap_leds] of_property_read_u32 failed\n");
            return -EINVAL;
    }
    pfs4412_leds -> led5con = ioremap(con,4);
    pfs4412_leds -> led5dat = ioremap(dat,4);
    pfs4412_leds -> led5bit = bit;
    of_node_put(pchild_node);
    if(!pfs4412_leds -> led5con || !pfs4412_leds -> led5dat){
        printk("[ioremap_leds] ioremap failed\n");
        return -EINVAL;
    }

    return 0;
}



/*设置led为输出模式*/
int set_output_leds(struct fs4412_leds *pfs4412_leds){

    //设置led为输出模式
    writel((readl(pfs4412_leds->led2con) & ~(0xf << pfs4412_leds->led2bit * 4))
                    | (1 << pfs4412_leds->led2bit * 4),pfs4412_leds->led2con);

    writel((readl(pfs4412_leds->led3con) & ~(0xf << pfs4412_leds->led3bit * 4))
                    | (1 << pfs4412_leds->led3bit * 4),pfs4412_leds->led3con);

    writel((readl(pfs4412_leds->led4con) & ~(0xf << pfs4412_leds->led4bit * 4))
                    | (1 << pfs4412_leds->led4bit * 4),pfs4412_leds->led4con);

    writel((readl(pfs4412_leds->led5con) & ~(0xf << pfs4412_leds->led5bit * 4))
                    | (1 << pfs4412_leds->led5bit * 4),pfs4412_leds->led5con);

    //关闭led
    writel(readl(pfs4412_leds->led2dat) & (~(1 << pfs4412_leds->led2bit)),pfs4412_leds->led2dat);
    writel(readl(pfs4412_leds->led3dat) & (~(1 << pfs4412_leds->led3bit)),pfs4412_leds->led3dat);
    writel(readl(pfs4412_leds->led4dat) & (~(1 << pfs4412_leds->led4bit)),pfs4412_leds->led4dat);
    writel(readl(pfs4412_leds->led5dat) & (~(1 << pfs4412_leds->led5bit)),pfs4412_leds->led5dat);

    return 0;
}

/*platform  probe操作函数*/
static int fs4412_leds_driver_probe(struct platform_device *pdev){

    dev_t devno;
    int ret = 0;

    printk("device tree brobed!\n");

    /*为设备申请空间*/
    pgfs4412_leds = (struct fs4412_leds*)kmalloc(sizeof(struct fs4412_leds),GFP_KERNEL);
    if(NULL == pgfs4412_leds){
        printk("[fs4412_leds_driver_probe] failed to kmalloc\n");
        return -1;
    }
    memset(pgfs4412_leds,0,sizeof(struct fs4412_leds));

    /*申请设备号*/
    devno = MKDEV(major,minor);
    ret = register_chrdev_region(devno,dev_num,"fs4412_leds");
    if(ret){
        ret = alloc_chrdev_region(&devno,minor,dev_num,"fs4412_leds");
        if(ret){
            kfree(pgfs4412_leds);
            pgfs4412_leds = NULL;
            printk("[fs4412_leds_driver_probe] failed to alloc_chrdev_region\n");
            return -1;
        }
        major = MAJOR(devno);
        minor = MINOR(devno);
    }
    printk("get major devno: %d ,minor devno: %d\n",major,minor);

    /*初始化设备，绑定操作函数集*/
    cdev_init(&pgfs4412_leds->cdev,&fs4412_leds_ops);
    pgfs4412_leds->cdev.owner = THIS_MODULE;
    ret = cdev_add(&pgfs4412_leds->cdev,devno,dev_num);
    if(ret){
        printk("[fs4412_leds_driver_probe] failed to cdev_add\n");
        unregister_chrdev_region(devno,dev_num);
        kfree(pgfs4412_leds);
        pgfs4412_leds = NULL;
        return ret;
    }

    /*自动mknod*/
    pgfs4412_leds->pcls = class_create(THIS_MODULE,"fs4412-leds");
    if(IS_ERR(pgfs4412_leds->pcls)){
        printk("[fs4412_leds_driver_probe] failed to class_create\n");
        cdev_del(&pgfs4412_leds->cdev);
        unregister_chrdev_region(devno,dev_num);
        kfree(pgfs4412_leds);
        pgfs4412_leds = NULL;
        return -1;
    }

    pgfs4412_leds->pdev = device_create(pgfs4412_leds->pcls,NULL,devno,NULL,"fs4412-leds");
    if(IS_ERR(pgfs4412_leds->pdev)){
        printk("[fs4412_leds_driver_probe] failed to class_create\n");
        cdev_del(&pgfs4412_leds->cdev);
        unregister_chrdev_region(devno,dev_num);
        class_destroy(pgfs4412_leds->pcls);
        kfree(pgfs4412_leds);
        pgfs4412_leds = NULL;
        return -1;
    }


    /*初始化led灯，主要分为两步：1、a-知道物理地址：地址映射 b-设备树：申请占用gpio ；2、设置GPIO为输出模式*/
    if(ioremap_leds(pdev,pgfs4412_leds)){
        device_destroy(pgfs4412_leds->pcls, devno);
        class_destroy(pgfs4412_leds->pcls);
        cdev_del(&pgfs4412_leds->cdev);
        unregister_chrdev_region(devno,dev_num);
        kfree(pgfs4412_leds);
        pgfs4412_leds = NULL;
        return -1;
    }

    /*设置led为输出模式*/
    set_output_leds(pgfs4412_leds);

    return 0;
}

/*platform  remove操作函数*/
static int fs4412_leds_driver_remove(struct platform_device *pdev){

    dev_t devno = MKDEV(major,minor);

    if(pgfs4412_leds) {

        if(pgfs4412_leds->pdev){
            device_destroy(pgfs4412_leds->pcls,devno);
        }
        if(pgfs4412_leds->pcls){
            class_destroy(pgfs4412_leds->pcls);           
        }
        cdev_del(&pgfs4412_leds->cdev);
        unregister_chrdev_region(devno,dev_num);

        iounmap_leds(pgfs4412_leds);
        kfree(pgfs4412_leds);
        pgfs4412_leds = NULL;
    }
    
    return 0;
}

/*设备树匹配名称列表*/
static const struct of_device_id fs4412_leds_dt_ids[] = {
    {.compatible = "fs4412,leds-reg"},
    { }
};
MODULE_DEVICE_TABLE(of,fs4412_leds_dt_ids);

/*id匹配名称列表*/
static const struct platform_device_id fs4412_leds_pltdev_ids[] = {
    {.name = "fs4412-leds",.driver_data = 0},
    { }
};
MODULE_DEVICE_TABLE(platform,fs4412_leds_pltdev_ids);

/*platform平台驱动*/
static struct platform_driver fs4412_leds_platform_driver = {

    .probe = fs4412_leds_driver_probe,
    .remove = fs4412_leds_driver_remove,
    .driver = {
        .name = "fs4412-leds",
        .of_match_table = of_match_ptr(fs4412_leds_dt_ids),
    },
    .id_table = fs4412_leds_pltdev_ids,
};

#if 0
module_platform_driver(fs4412_leds_platform_driver);

#else
static int __init fs4412_leds_driver_init(void){
    platform_driver_register(&fs4412_leds_platform_driver);
    return 0;
}

static void __exit fs4412_leds_driver_exit(void){
    platform_driver_unregister(&fs4412_leds_platform_driver);
}

module_init(fs4412_leds_driver_init);
module_exit(fs4412_leds_driver_exit);
#endif

MODULE_LICENSE("GPL");