#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

#include "mychar.h"

#define MAJ_NUM 11
#define MIN_NUM 0 
#define DEV_NUM 1

#define BUF_LEN 100


struct mychar_dev{
	struct cdev mydev;
	char mydev_buf[BUF_LEN];
	int curlen;
};

struct mychar_dev gmydev;

int major = MAJ_NUM;
int minor = MIN_NUM;
int dev_num = DEV_NUM;

//open一般对设备上硬件进行初始化，简单设备只需要return 0;
int mychar_open(struct inode *pnode,struct file *pfile){
	printk("mychar_open is called\n");
	gmydev.curlen = 0;
	pfile->private_data = (void*)(container_of(pnode->i_cdev,struct mychar_dev,mydev));

	return 0;
}

int mychar_close(struct inode *pnode,struct file *pfile){

	printk("mychar_close is called\n");
	return 0;
}

//对应用户空间函数ssize_t read(int fd, void *buf, size_t count);
ssize_t mychar_read(struct file* pfile, char __user * puser,size_t count,loff_t *p_pos){
	
	struct mychar_dev *pmydev;
	int size = 0;
	int ret = 0;
	pmydev = (struct mychar_dev*)pfile->private_data;
	//一次读取的字节数
	//读取的数据量比存的少
	if(count < pmydev->curlen){
		size = count;
	}else{
		size = pmydev->curlen;
	}
	ret = copy_to_user(puser,pmydev->mydev_buf,size);
	if(ret){
		printk("copy_to_user failed\n");
		return -1;
	}	
	
	//前移数据
	memcpy(pmydev->mydev_buf,pmydev->mydev_buf+size,pmydev->curlen-size);
	pmydev->curlen -= size;
	return size;
}

ssize_t mychar_write(struct file* pfile,const char __user * puser,size_t count,loff_t *p_pos){

		
	struct mychar_dev *pmydev;
	int size = 0;
	int ret = 0;
	pmydev = (struct mychar_dev*)pfile->private_data;
	//实际写入的数据量
	if(count < BUF_LEN - pmydev->curlen){
		size = count;
	}else{
		size = BUF_LEN - pmydev->curlen;
	}

	ret = copy_from_user(pmydev->mydev_buf+pmydev->curlen,puser,size);
	if(ret){
		printk("failed to copy_from_user\n");
		return -1;
	}
	pmydev->curlen += size;
	return size;
}


long mychar_ioctl(struct file* flip,unsigned int cmd,unsigned long arg){
	struct mychar_dev *pmydev = (struct mychar_dev*)flip->private_data;
	int __user* pret = (int *)arg;
	int ret = 0;
	int maxlen = BUF_LEN;

	switch(cmd)
	{
		case MY_CHAR_IOCTL_GET_MAXLEN:
			ret = copy_to_user(pret,&maxlen,sizeof(int));
			if(ret){
				printk("failed to copy_to_user MAXLEN\n");
					return -1;
			}
			break;

		case MY_CHAR_IOCTL_GET_CURLEN:
			ret = copy_to_user(pret,&pmydev->curlen,sizeof(int));
			if(ret){
				printk("failed to copy_to_user CURLEN\n");
					return -1;
			}
			break;
		default:
			printk("unkoow cmd\n");
			return -1;
	}

	return 0;
}

struct file_operations ops = {
	.owner = THIS_MODULE,
	.open = mychar_open,
	.release = mychar_close,
	.read = mychar_read,
	.write = mychar_write,
	.unlocked_ioctl = mychar_ioctl,
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
	
	cdev_init(&gmydev.mydev,&ops);
	gmydev.mydev.owner = THIS_MODULE;
	cdev_add(&gmydev.mydev,devno,1);

	printk("mychar load\n");
	return 0;
}

void __exit mychar_exit(void){

	dev_t devno = MKDEV(major,minor);
	cdev_del(&gmydev.mydev);
	unregister_chrdev_region(devno,dev_num);
	printk("mychar exit\n");
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("GHC");
MODULE_DESCRIPTION("test mydev");

module_init(mychar_init);
module_exit(mychar_exit);

