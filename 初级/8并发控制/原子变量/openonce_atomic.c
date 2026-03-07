#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define DEV_NUM 1

int major = 11;
int minor = 0;
int dev_num = DEV_NUM;

struct openonce_dev{
    struct cdev cdev;
    atomic_t openflag;
};

struct openonce_dev gmydev;

int openonce_open(struct inode* pnode,struct file* pfile){
    
    struct openonce_dev * pmydev = NULL;
    pfile->private_data = (void *)(container_of(pnode->i_cdev,struct openonce_dev,cdev));
    pmydev = (struct openonce_dev *)pfile->private_data;

    if(atomic_dec_and_test(&pmydev->openflag)){
        return 0;
    }else{
        atomic_inc(&pmydev->openflag);
        printk("The device is opend already\n");
        return -1;
    }

    return 0;
}

int openonce_close(struct inode* pnode,struct file* pfile){

    struct openonce_dev * pmydev = (struct openonce_dev *)pfile->private_data;
    atomic_set(&pmydev->openflag,1);
    return 0;
}



struct file_operations ops = {
    .owner = THIS_MODULE,
    .open = openonce_open,
    .release = openonce_close,
};


int __init openonce_init(void){

    int ret;
    dev_t devno = MKDEV(major,minor);

    ret = register_chrdev_region(devno,dev_num,"openonce");
    if(ret){
        ret = alloc_chrdev_region(&devno,minor,dev_num,"openonce");
        if(ret){
            printk("alloc devno failed!\n");
            return -1;
        }
        major = MAJOR(devno);
        minor = MINOR(devno);
        printk("alloc devno :%d\n",major);
    }

    cdev_init(&gmydev.cdev,&ops);
    gmydev.cdev.owner = THIS_MODULE;
    cdev_add(&gmydev.cdev,devno,1);


    atomic_set(&gmydev.openflag, 1);
    printk("openonce will load\n");

    return 0;
}


void __exit openonce_exit(void){

    dev_t devno = MKDEV(major,minor);
    cdev_del(&gmydev.cdev);

    unregister_chrdev_region(devno,dev_num);

    printk("openonce eill exit\n");
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("GHC");
MODULE_DESCRIPTION("原子变量测试代码");

module_init(openonce_init);
module_exit(openonce_exit);