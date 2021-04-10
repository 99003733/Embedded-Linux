#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
static DEFINE_MUTEX(m1);
static struct task_struct *task1;
static struct task_struct *task2;

int count = 50;
int i;
module_param(count, int, S_IRUGO);

static int val = 100;
static int thread_one(void *pargs) {
  int i;
  //mutex_lock(&m1);
  for (i = 1; i <= count; i++) {
    printk("Thread A--%d\n", i);
    val++;
  }
  //mutex_unlock(&m1);
  return 0;
}

static int thread_two(void *pargs) {
  mutex_lock(&m1);
  int i;
  for (i = 1; i <= count; i++) {
    printk("Thread B--%d\n", i);
    val--;
  }
  mutex_unlock(&m1);
  return 0;
}

static int __init wqdemo_init(void) {

  mutex_init(&m1);

  task1 = kthread_run(thread_one, NULL, "thread_A");
  task2 = kthread_run(thread_two, NULL, "thread_B");
  printk("Thread Demo..welcome\n");
  return 0;
}
static void __exit wqdemo_exit(void) { // cleanup_module

  printk("Thread Demo,Leaving the world,val=%d\n", val);
}

module_init(wqdemo_init);
module_exit(wqdemo_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("I am Aditya");
MODULE_DESCRIPTION("mutex");
