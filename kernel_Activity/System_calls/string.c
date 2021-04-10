#include<linux/syscalls.h>
#include<linux/kernel.h>
#include<linux/uaccess.h>
#include<linux/linkage.h>

SYSCALL_DEFINE1(mys, char*, stri)
{  
 int n;
 char buf[100];
 n=copy_from_user(buf,stri,100);
 if(n)
 {
    printk("error");
    -EFAULT;
 }
 printk("mysys_string is to be given %s\n", buf);
 
 return 0;
}
