#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/string.h>
#include<asm/uaccess.h>
#include <linux/ioctl.h>

#define QUERY_GET_VALUE    _IOR(‘q’, 1, int *)
#define QUERY_CLEAR_VALUE  _IO(‘q’, 2)
#define QUERY_SET_VALUE    _IOW(‘q’, 3, int *)

//static char buf[100]; //driver local buffer

static int a;

int pseudo_open(struct inode* inode , struct file* file)
{
	printk("Pseudo--open method\n");
	return 0;
}
int pseudo_close(struct inode* inode , struct file* file)
{
	printk("Pseudo--release method\n");
	return 0;
}
ssize_t pseudo_read(struct file * file, char __user * buf , size_t size, loff_t * off)
{
	printk("Pseudo--read method\n");
	return 0;
}
ssize_t pseudo_write(struct file * file, const char __user * buf , size_t size, loff_t * off)
{
	printk("Pseudo--write method\n");
	return -ENOSPC;
}

//structure containing device operation

 

static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{

	switch(cmd)
	{

		case QUERY_GET_VALUE:
			copy_to_user((int*)arg,&a,sizeof(int));
			break;
		case QUERY_SET_VALUE:
			copy_from_user(&a,(int *)arg,sizeof(int));
			break;
		case QUERY_CLEAR_VALUE:
			break;
	}
	return 0;

}

struct file_operations fops = {
.open = pseudo_open,
.release = pseudo_close,
.write = pseudo_write,
.read = pseudo_read,
.unlocked_ioctl = my_ioctl
};

static int hello_init(void) //init function to be called at the time of insmod
{

	int t=register_chrdev(90,"mydev",&fops);

	if(t<0)
        {
		printk(KERN_ALERT "device registration failed.");
        }
	else
        {
		printk(KERN_ALERT "device registred\n");
        }
	return 0;

}

static void hello_exit(void) //exit function to be called at the time of rmmod
{

	unregister_chrdev(90,"mydev");
	printk(KERN_ALERT "exit");
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("I am aditya");
MODULE_DESCRIPTION("Parameter demo Module");
