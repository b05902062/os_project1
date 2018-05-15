#include<linux/linkage.h>
#include<linux/kernel.h>


asmlinkage long sys_printinfo(int pid,int start_time_int,int start_time_fraction,int end_time_int,int end_time_fraction){

           
printk("[os_project1] pid=%d, start = %d.%d, end = %d.%d\n",pid,start_time_int,start_time_fraction, end_time_int, end_time_fraction);



return((long)0);

}
