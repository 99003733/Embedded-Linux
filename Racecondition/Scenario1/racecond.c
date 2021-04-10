#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>  // for threads
#include <linux/time.h>   // for using jiffies 
#include <linux/timer.h>
#include <linux/delay.h>



static struct task_struct *task1;
static struct task_struct *task2;


int val=100;
const int max=10000;
static int thread_one(void *pargs)
{
	int i;
	for(i=1;i<=max;i++)
	{
		val++;
		//printk("Thread A--%d\n",val);
		if(kthread_should_stop())
			break;
	}
	do_exit(0);
	return 0;
}
static int thread_two(void *pargs)
{
	int i;
	for(i=1;i<=max;i++)
	{
		val--;
		//printk("Thread B--%d\n",val);
		if(kthread_should_stop())
			break;
	}
	do_exit(0);
	return 0;
}


// Module Initialization
static int __init init_thread(void)
{
    printk(KERN_INFO "Creating Thread\n");
    //Create the kernel thread with name 'mythread'
    task1 = kthread_run(thread_one, NULL, "thread_A");
    if (task1)
        printk(KERN_INFO "Thread Created A successfully\n");
        
    else
        printk(KERN_ERR "Thread creation A failed\n");
        
    task2 = kthread_run(thread_two, NULL, "thread_B");
    if (task2)
    {
        printk(KERN_INFO "Thread Created B successfully\n");
    }
    else
    {
        printk(KERN_ERR "Thread creation B failed\n");
     }
    return 0;
}
// Module Exit
static void __exit cleanup_thread(void)
{
   printk(KERN_INFO "Cleaning Up\n");
   if (task1)
   {
       kthread_stop(task1);
       printk("Thread A--%d\n",val);
       printk(KERN_INFO "Thread stopped");
   }
   if (task2)
   {
       kthread_stop(task2);
       printk("Thread B--%d\n",val);
       printk(KERN_INFO "Thread stopped");
   }
   
}
MODULE_LICENSE("GPL");
module_init(init_thread);
module_exit(cleanup_thread);
