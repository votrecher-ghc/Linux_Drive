#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

#define MAJ_NUM 11
#define MIN_NUM 0 
#define DEV_NUM 1

#define BUF_LEN 100

int major = MAJ_NUM;
int minor = MIN_NUM;
int dev_num = DEV_NUM;

char mydev_buf[BUF_LEN];
int curlen = 0;

//open一般对设备上硬件进行初始化，简单设备只需要return 0;
int mychar_open(struct inode *pnode,struct file *pfile){
	printk("mychar_open is called\n");
	return 0;
}

int mychar_close(struct inode *pnode,struct file *pfile){
	printk("mychar_close is called\n");
	return 0;
}

//对应用户空间函数ssize_t read(int fd, void *buf, size_t count);
ssize_t mychar_read(struct file* pfile, char __user * puser,size_t count,loff_t *p_pos){
	
	//一次读取的字节数
	int size = 0;
	int ret = 0;
	//读取的数据量比存的少
	if(count < curlen){
		size = count;
	}else{
		size = curlen;
	}
	ret = copy_to_user(puser,mydev_buf,size);
	if(ret){
		printk("copy_to_user failed\n");
		return -1;
	}	
	
	//前移数据
	memcpy(mydev_buf,mydev_buf+size,curlen-size);
	curlen -= size;
	return size;
}

ssize_t mychar_write(struct file* pfile,const char __user * puser,size_t count,loff_t *p_pos){
	
	//实际写入的数据量
	int size = 0;
	int ret = 0;
	if(count < BUF_LEN - curlen){
		size = count;
	}else{
		size = BUF_LEN - curlen;
	}

	ret = copy_from_user(mydev_buf+curlen,puser,size);
	if(ret){
		printk("failed to copy_from_user\n");
		return -1;
	}
	curlen += size;
	return size;
}

struct cdev mydev;

struct file_operations ops = {
	.owner = THIS_MODULE,
	.open = mychar_open,
	.release = mychar_close,
	.read = mychar_read,
	.write = mychar_write,
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

