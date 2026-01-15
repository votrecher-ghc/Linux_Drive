#include <linux/module.h>
#include <linux/kernel.h>


int __init myhello_init(void){

	printk("myhello load\n");
	return 0;
}

void __exit myhello_exit(void){

	printk("myhello exit\n");
}


MODULE_AUTHOR("GHC");
MODULE_DESCRIPTION("test module");
MODULE_LICENSE("GPL");

module_init(myhello_init);
module_exit(myhello_exit);

