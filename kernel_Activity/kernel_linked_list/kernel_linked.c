#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/kthread.h>             //kernel threads
#include <linux/sched.h>               //task_struct 
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/kfifo.h>



 
#define MAX_BUF_SIZE 1024 

struct mutex etx_mutex;
unsigned long etx_global_variable = 0;
 
dev_t pseudo_dev_id;
int ndevices=5;
module_param(ndevices,int,S_IRUGO);

dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
 
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
 
static struct task_struct *etx_thread1;
static struct task_struct *etx_thread2; 
static struct task_struct *etx_thread3; 
/*************** Driver functions **********************/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, 
                char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, 
                const char *buf, size_t len, loff_t * off);
 /******************************************************/
 
int thread_function1(void *pv);
int thread_function2(void *pv);
 
int thread_function1(void *pv)
{
    
    while(!kthread_should_stop()) {
        mutex_lock(&etx_mutex);
        etx_global_variable++;
        int ret,i=0;
	c_dev *pobj;
	pseudo_cls=class_create(THIS_MODULE,"pseudo_class");
	ret=alloc_chrdev_region(&pseudo_dev_id,0,ndevices,"pseudo_char_driver");
	if(ret)
	{
		printk("error in alloc....\n");
		return -EINVAL; //suitable err value
	}
	//per device initialization starts here
	for(i=0;i<ndevices;i++)
	{
		pobj=kmalloc(sizeof(c_dev),GFP_KERNEL);
		if(pobj==NULL)
		{
			return -EINVAL;
		}
		ret=kfifo_alloc(&pobj -> kfifo,MAX_BUF_SIZE,GFP_KERNEL);
		if(ret)
		{
			return -EINVAL;
		}
		cdev_init(&pobj->cdev, &pseudo_fops);
		kobject_set_name(&pobj->cdev.kobj,"pdevice%d",i);
		ret=cdev_add(&pobj -> cdev,pseudo_dev_id+i,1);
		if(ret)
		{
			return -EINVAL;
		}
		pobj->pnode=device_create(pseudo_cls,NULL,pseudo_dev_id+i,NULL,"psample%d",i);
		list_add_tail(&pobj->lentry,&dev_list);	
	}
	printk("pseudo char drivered registered,major=%d\n",MAJOR(pseudo_dev_id));
        pr_info("In EmbeTronicX Thread Function1 %lu\n", etx_global_variable);
        mutex_unlock(&etx_mutex);
        msleep(1000);
    }
    return 0;
}
 
int thread_function2(void *pv)
{
    while(!kthread_should_stop()) {
        mutex_lock(&etx_mutex);
        etx_global_variable++;
        pr_info("In EmbeTronicX Thread Function2 %lu\n", etx_global_variable);
        mutex_unlock(&etx_mutex);
        msleep(1000);
    }
    return 0;
}
int thread_function3(void *pv)
{
    while(!kthread_should_stop()) {
        mutex_lock(&etx_mutex);
        etx_global_variable++;
        pr_info("In EmbeTronicX Thread Function2 %lu\n", etx_global_variable);
        mutex_unlock(&etx_mutex);
        msleep(1000);
    }
    return 0;
}
 
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = etx_read,
        .write          = etx_write,
        .open           = etx_open,
        .release        = etx_release,
};

typedef struct priv_obj1{
	struct list_head lentry;
	void *pbuffer;				
	struct kfifo kfifo;			
	struct cdev cdev; 			
	struct device* pnode;			
}c_dev;
 
LIST_HEAD(dev_list); 
 
static int etx_open(struct inode *inode, struct file *file)
{
        pr_info("Device File Opened...!!!\n");
        struct c_dev *pobj=container_of(inode->i_cdev,c_dev,cdev);
	file->private_data=pobj;
        return 0;
}
 
static int etx_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        return 0;
}
 
static ssize_t etx_read(struct file *file, 
                char __user *buf, size_t len, loff_t *off)
{
        pr_info("Read function\n");
        C_DEV* pobj=file->private_data;
	printk("entered pseudo read method\n");
	int klen,rcount,ret,nbytes;
	klen=kfifo_len(&pobj->kfifo);
	if(klen==0)
	{
		printk("device is empty\n");
		return 0;
	}
	rcount=len;
	if(klen<rcount) rcount=klen;	//rcount=min(klen,len)
	if(access_ok(VERIFY_WRITE,(void __user*)buf,(unsigned long)rcount))
	{
		nbytes=kfifo_out(&pobj->kfifo,buf,rcount);
	}
	else
		return -EFAULT;
	printk("read method completed\n");
	return rcount;
 
}
static ssize_t etx_write(struct file *file, 
                const char __user *buf, size_t len, loff_t *off)
{
        pr_info("Write Function\n");
        c_dev* pobj=file->private_data;
	printk("entered pseudo write method\n");
	int wcount,ret,kremain;
	kremain=kfifo_avail(&pobj->kfifo);
	if(kremain==0)
	{
		printk("no space left on device\n");
		return -ENOSPC;
	}
	wcount=len;
	if(kremain<wcount) wcount=kremain;	//min(len,kremain)
	if(access_ok(VERIFY_READ,(void __user*)buf,(unsigned long)wcount))
	{
		kfifo_in(&pobj->kfifo,buf,wcount);
	}
	else
		return -EFAULT;
	return wcount;
}
 
static int __init etx_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
                pr_info("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&etx_cdev,&fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&etx_cdev,dev,1)) < 0){
            pr_info("Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
            pr_info("Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL){
            pr_info("Cannot create the Device \n");
            goto r_device;
        }
   
        mutex_init(&etx_mutex);
        
        /* Creating Thread 1 */
        etx_thread1 = kthread_run(thread_function1,NULL,"eTx Thread1");
        if(etx_thread1) {
            pr_err("Kthread1 Created Successfully...\n");
        } else {
            pr_err("Cannot create kthread1\n");
             goto r_device;
        }
 
         /* Creating Thread 2 */
        etx_thread2 = kthread_run(thread_function2,NULL,"eTx Thread2");
        if(etx_thread2) {
            pr_err("Kthread2 Created Successfully...\n");
        } else {
            pr_err("Cannot create kthread2\n");
             goto r_device;
        }
        etx_thread3 = kthread_run(thread_function3,NULL,"eTx Thread3");
        if(etx_thread3) {
            pr_err("Kthread3 Created Successfully...\n");
        } else {
            pr_err("Cannot create kthread2\n");
             goto r_device;
        }
        
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;
 
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        cdev_del(&etx_cdev);
        return -1;
}
 
static void __exit etx_driver_exit(void)
{
/*
	c_dev *pobj,*psafe;
	int i=0;
	list_for_each_entry_safe(pobj,psafe,&dev_list,lentry)
	{
		kfifo_free(&pobj->kfifo);	//dont deallocate pbuffer again using kfree
		cdev_del(&pobj->cdev);
		device_destroy(pseudo_cls,pseudo_dev_id+i);
		kfree(pobj);
		i++;
	}
	//common cleanup
 	unregister_chrdev_region(pseudo_dev_id,ndevices);
	class_destroy(pseudo_cls);
	printk("device unregistered\n");*/
	
        kthread_stop(etx_thread1);
        kthread_stop(etx_thread2);
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&etx_cdev);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver Remove...Done!!\n");
}
 
module_init(etx_driver_init);
module_exit(etx_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <embetronicx@gmail.com>");
MODULE_DESCRIPTION("A simple device driver - Mutex");
MODULE_VERSION("1.17");
