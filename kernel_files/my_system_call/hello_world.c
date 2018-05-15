#include<linux/linkage.h>
#include<linux/kernel.h>
asmlinkage long sys_hello_world(int arg0){

	printk("hello world your argument %d\n",arg0);

	return (long)0;
}
