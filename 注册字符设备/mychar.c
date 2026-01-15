#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define MAJ_NUM 11
#define MIN_NUM 0 
#define DEV_NUM 1


int major = MAJ_NUM;
int minor = MIN_NUM;
int dev_num = DEV_NUM;

struct cdev mydev;

struct file_operations ops = {
	.owner = THIS_MODULE,

};

int init_chardev(void){

	dev_t devno = MKDEV(major,minor);

	int ret = register_chrdev_region(devno,dev_num,"mychar");
	if(ret){
		ret = alloc_chrdev_region(&devno,minor,dev_num,"mychar");
		if(ret){
			printk("get devno failed\n");
			return -1;
		}
		major = MAJOR(devno);
		minor = MINOR(devno);
	}
	return devno;
}

int __init mychar_init(void){

	dev_t devno = init_chardev();
	if(devno == -1){
		printk("get devno failed\n");
		return -1;
	}
	//printk("get devno:%d",MAJOR(devno));
	
	cdev_init(&mydev,&ops);
	mydev.owner = THIS_MODULE;
	cdev_add(&mydev,devno,1);

	printk("mychar load\n");
	return 0;
}

void __exit mychar_exit(void){

	dev_t devno = MKDEV(major,minor);
	cdev_del(&mydev);
	unregister_chrdev_region(devno,dev_num);
	printk("mychar exit\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GHC");
MODULE_DESCRIPTION("test mydev");

module_init(mychar_init);
module_exit(mychar_exit);

