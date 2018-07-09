#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/timer.h>

struct timer_list my_timer;

void my_timer_func(unsigned long arg)
{
  pr_info("Current Task %s, PID %d\r\n", current->comm, current->pid);
  pr_info("%s with data = %d\r\n", __func__, (int)arg);
  /* Reload Timer with 1 Second */
  mod_timer(&my_timer, jiffies + HZ); 
}

static int __init device_init(void)
{
  int data = 100;

  /* Create an Instance of a New Timer */
  init_timer(&my_timer);

  /* Register Timer Handler */
  my_timer.function = my_timer_func;

  /* 1 Second Time Period */
  my_timer.expires  = jiffies + HZ;

  /* Argument for Timer Handler */
  my_timer.data     = data;

  /* Start Timer */ 
  add_timer(&my_timer);

  pr_info("%s\r\n", __func__);
  return 0;
}

static void __exit device_exit(void)
{
  /* Delete Timer Instance (SMP Safe) */
  del_timer_sync(&my_timer);
  pr_info("%s\r\n", __func__);
}

/* Register the names of custom entry and exit routines */
module_init(device_init);
module_exit(device_exit);

/* Comments which are retained in code */
MODULE_AUTHOR("debmalyasarkar1@gmail.com");
MODULE_DESCRIPTION("Timer as Bottom Half");
MODULE_LICENSE("GPL");

