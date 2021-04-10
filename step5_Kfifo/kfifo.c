#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kfifo.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

 

#define MAX_SIZE 1024

 

unsigned char *pbuffer;


struct kfifo myfifo;
int rcount;
int ret;
int wcount;
int pseudo_open(struct inode *inode, struct file *file) {
  printk("Pseudo--open method\n");
  return 0;
}
int pseudo_close(struct inode *inode, struct file *file) {
  printk("Pseudo--release method\n");
  return 0;
}
ssize_t pseudo_read(struct file *file, char __user *buf, size_t size, loff_t *off) 
{
  printk("Pseudo--read method\n");
  // Read method:-
  if (kfifo_is_empty(&myfifo)) {
    printk("buffer is empty\n");
    return 0;
  }
  rcount = size;
  if (rcount > kfifo_len(&myfifo))
  {
    rcount = kfifo_len(&myfifo);
  }
  char *tbuf = kmalloc(rcount, GFP_KERNEL);
  kfifo_out(&myfifo, tbuf, rcount);
  ret = copy_to_user(buf, tbuf, rcount);
  // error handling
  //kfree(tbuf);
  return 0;
}
ssize_t pseudo_write(struct file *file, const char __user *buf, size_t size, loff_t *off) 
{
  printk("Pseudo--write method\n");
  // Write method:-
  if (kfifo_is_full(&myfifo)) {
    printk("buffer is full\n");
    return -ENOSPC;
  }
  wcount = size;
  if (wcount > kfifo_avail(&myfifo))
  {
    wcount = kfifo_avail(&myfifo);
  }
  char *tbuf = kmalloc(wcount, GFP_KERNEL);
  ret = copy_from_user(tbuf, buf, wcount);
  // error handling if copy_form_user
  kfifo_in(&myfifo, tbuf, wcount);
  //kfree(tbuf);
  return 0;
  /*return -ENOSPC; */
}

 

struct cdev cdev;
int ndevices = 1;
struct file_operations fops = {.open = pseudo_open,
                               .release = pseudo_close,
                               .write = pseudo_write,
                               .read = pseudo_read
                               };

 

dev_t pdevid;

static int __init psuedo_init(void) {
  int ret;
  int i = 0;
  ret = alloc_chrdev_region(&pdevid, 0, ndevices, "pseudo_sample");
  if (ret) 
  {
    printk("Pseudo: Failed to register driver\n");
    return -EINVAL;
  }
  pbuffer = kmalloc(MAX_SIZE, GFP_KERNEL);
  kfifo_init(&myfifo, pbuffer, 11);
  // kfifo_alloc(&kfifo, MAX_SIZE, GFP_KERNEL);
  cdev_init(&cdev, &fops);
  kobject_set_name(&cdev.kobj, "pdevice%d", i);
  ret = cdev_add(&cdev, pdevid, 1);
  printk("Successfully registered,major=%d,minor=%d\n", MAJOR(pdevid), MINOR(pdevid));
  printk("Pseudo Driver Sample..welcome\n");
  return 0;
}
static void __exit psuedo_exit(void) {
  cdev_del(&cdev);
  unregister_chrdev_region(pdevid, ndevices);
  //kfifo_free(myfifo);
  printk("Pseudo Driver Sample..Bye\n");
}

 

module_init(psuedo_init);
module_exit(psuedo_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Aditya Mewara");
MODULE_DESCRIPTION("Register Char Driver");
