#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>


int major = 11;	//主设备号
int minor = 0;	//次设备号
int dev_num = 1;	//设备数量


int __init mychar_init(void){

	//构造设备号
	dev_t devno = MKDEV(major,minor);


	/* 从 devno 这个号码开始，连续占用 dev_num 个设备号: devno、devno+1、devno+2，
		 mychar会出现在/proc/devices中，返回 0：成功，返回 负数：失败 */
	int ret = register_chrdev_region(devno,dev_num,"mychar");

	if(ret){
		ret = alloc_chrdev_region(&devno,minor,dev_num,"mychar");
		if(ret){
			printk("申请设备号失败\n");
			return -1;
		}

		printk("申请主设备号:%d 失败，系统自动分配设备号：%d", major, MAJOR(devno));

		major = MAJOR(devno);
		minor = MINOR(devno);
	}

	printk("申请到设备号:%d", major);
	printk("mychar will load\n");
	return 0;
}

void __exit mychar_exit(void)
{
	dev_t devno = MKDEV(major,minor);
	unregister_chrdev_region(devno,dev_num);
	printk("mychar will exit\n");

}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GHC");
MODULE_DESCRIPTION("test get devno");

module_init(mychar_init);
module_exit(mychar_exit);

