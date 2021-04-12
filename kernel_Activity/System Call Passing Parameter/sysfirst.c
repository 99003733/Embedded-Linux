#include<linux/kernel.h>
#include<linux/syscalls.h>

SYSCALL_DEFINE4(mysyscall,int,a,int,b,int,c,int,d)
{
	printk("This is Test call x=%d y=%d" ,a,b,c,d);
	return a+b+c+d;
}
