#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/semaphore.h>
#include <linux/kthread.h>

static DEFINE_SEMAPHORE (s1);
static DEFINE_SEMAPHORE (s2);
static struct task_struct *task1;
static struct task_struct *task2;

int count=50;
module_param(count,int,S_IRUGO);

//wait_queue_head_t w1;
//wait_queue_head_t w2;

static int val=100;
static int buflen=0;
static int k=0;

static int thread_one(void *pargs)
{
    down_interruptible(&s1);
    down_interruptible(&s2);
 
    int i;
    for(i=1;i<=count;i++)
    {
        //wait_event_interruptible(w1, (k > 0) );
        printk("Consumer--%d\n",i);
        k--;
        //if(k==0) wake_up_interruptible(&w2);
    }
        up(&s2);
	return 0;
}

static int thread_two(void *pargs)
{
    int i;
    down_interruptible(&s2);
    for(i=1;i<=count;i++)
    {
        //wait_event_interruptible(w2, (k < 5) );
        printk("Producer--%d\n",i);
        k++;
        //if(k>=4) wake_up_interruptible(&w1);
    }
        up(&s2);
        up(&s1);
 	return 0;
}

static int __init wqdemo_init(void) {        //init_module
  sema_init(&s1,0); // consumer
  sema_init(&s2,1); // producer
  //init_waitqueue_head(&w1);
  //init_waitqueue_head(&w2);
  task1=kthread_run(thread_one, NULL, "thread_A");  //kthread_create + wake_up_process
  task2=kthread_run(thread_two, NULL, "thread_B");
  printk("Thread Demo..welcome\n");
  return 0;
}
static void __exit wqdemo_exit(void) {       //cleanup_module
  printk("Thread Demo,Leaving the world,val=%d\n",val);
}

module_init(wqdemo_init);
module_exit(wqdemo_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aditya Mewara");
MODULE_DESCRIPTION("Thread Example Module");
