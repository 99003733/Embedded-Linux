#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>


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

struct cdev cdev;


struct file_operations fops = {
	.open = pseudo_open,
	.release = pseudo_close,
	.write = pseudo_write,
	.read = pseudo_read
};

struct device *pdev; //global
struct class *pclass; //global
int ndevices=1;
dev_t pdevid;
static int __init psuedo_init(void)
{
	
	int ret;
	int i=0;
	ret=alloc_chrdev_region(&pdevid, 0, ndevices, "pseudo_sample");
	if(ret) 
	{
		printk("Pseudo: Failed to register driver\n");
		return -EINVAL;
	}
	pclass = class_create(THIS_MODULE, "pseudo_class");
	cdev_init(&cdev, &fops);
	
	kobject_set_name(&cdev.kobj,"pdevice%d",i);
	
	ret = cdev_add(&cdev, pdevid, 1);
	
	
	//alloc_chrdev_region, cdev_init, cdev_add
	pdev = device_create(pclass, NULL, pdevid, NULL, "psample%d",i);
	
	
	printk("Successfully registered,major=%d, minor=%d\n",MAJOR(pdevid), MINOR(pdevid));
	printk("Pseudo Driver Sample..welcome\n");
	return 0;
}
static void __exit psuedo_exit(void) {
	cdev_del(&cdev);
	unregister_chrdev_region(pdevid, ndevices);
	printk("Pseudo Driver Sample..Bye\n");
	device_destroy(pclass, pdevid);
	class_destroy(pclass);
}

module_init(psuedo_init);
module_exit(psuedo_exit);
