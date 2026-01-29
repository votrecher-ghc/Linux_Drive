#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>

#define DEV_NUM 1
#define OPEN_DEVICES_NUM 3

int major = 11;
int minor = 0;
int dev_num = DEV_NUM;


struct openonce_dev{
    struct cdev mydev;
    int openflag;
    spinlock_t lock;
};

struct openonce_dev gmydev;


int openonce_open(struct inode* pnode, struct file * pfile)
{
    struct openonce_dev* pmydev = NULL;
    pfile->private_data = (void *)(container_of(pnode->i_cdev,struct openonce_dev,mydev));

    pmydev = (struct openonce_dev *)pfile->private_data;

    spin_lock(&pmydev->lock);
    if(pmydev->openflag > 0){
        pmydev->openflag--;
        spin_unlock(&pmydev->lock);
    }else{
        spin_unlock(&pmydev->lock);
        printk("The device is opened already\n");
        return -1;
    }
    return 0;
}

int openonce_close(struct inode* pnode, struct file * pfile)
{
    struct openonce_dev* pmydev = NULL;
    pmydev = (struct openonce_dev *)pfile->private_data;

    spin_lock(&pmydev->lock);
    pmydev->openflag++;
    spin_unlock(&pmydev->lock);

   return 0;
}


struct file_operations ops = {
    .owner = THIS_MODULE,
    .open = openonce_open,
    .release = openonce_close,
};

int __init openonce_init(void){

    int ret = 0;
    dev_t devno = MKDEV(major,minor);
    ret = register_chrdev_region(devno,dev_num,"openonce");
    if(ret){
        ret = alloc_chrdev_region(&devno,minor,dev_num,"openonce");
        if(ret){
            printk("alloc devno failed \n");
            return -1;
        }
        major = MAJOR(devno);
        minor = MINOR(devno);
        printk("alloc devno :%d\n",major);
    }


    cdev_init(&gmydev.mydev,&ops);
    gmydev.mydev.owner = THIS_MODULE;
    cdev_add(&gmydev.mydev,devno,dev_num);

    gmydev.openflag = OPEN_DEVICES_NUM;

    spin_lock_init(&gmydev.lock);
    printk("openonce load\n");
    return 0;
}

void __exit openonce_exit(void){

    dev_t devno = MKDEV(major, minor);
    cdev_del(&gmydev.mydev);

    unregister_chrdev_region(devno,dev_num);
    printk("openonce exit\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GHC");
MODULE_DESCRIPTION("自旋锁测试程序");

module_init(openonce_init);
module_exit(openonce_exit);