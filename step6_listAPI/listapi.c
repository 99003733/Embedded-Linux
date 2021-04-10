#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/slab.h>

struct sample {
  int x;
  int y;
  int z;
  struct list_head entry;
};

LIST_HEAD(mylist);

int __init listdemo_init(void) { 
  int i;
  struct sample *ps;
  for (i = 1; i <= 10; i++) {
    ps = kmalloc(sizeof(struct sample), GFP_KERNEL);
    ps->x = i * 100;
    ps->y = i * 100 + 10;
    list_add_tail(&ps->entry, &mylist);
  }
  print_data();
  printk("Hello World..welcome to the world\n");
  return 0;
}

void print_data(void) {
  struct list_head *pcur;
  struct sample *ps;
  list_for_each(pcur, &mylist) {
    ps = list_entry(pcur, struct sample, entry); 
  }
}

void __exit listdemo_exit(void) { 
  struct list_head *pcur, *pbak;
  struct sample *ps;
  list_for_each_safe(pcur, pbak, &mylist) {
    ps = list_entry(pcur, struct sample, entry); 
    kfree(ps);
  }
  printk("Bye,Bye\n");
}

module_init(listdemo_init);
module_exit(listdemo_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("I am Aditya");
MODULE_DESCRIPTION("A Hello, World Module")
