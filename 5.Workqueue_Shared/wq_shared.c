#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/sched.h>

/* Workqueue Function - BH Routine */
static void w_func(struct work_struct *w_arg)
{
  pr_info("Current Task %s, PID %d\r\n",current->comm, current->pid);
}

/* Create struct work_struct instance and initialize it with BH routine  */
static DECLARE_WORK(work, w_func);

/* Schedule work object by adding it to workqueue(shared). */
static int __init device_init(void)
{
  schedule_work(&work);
  pr_info("%s scheduled work\r\n", __func__);
  return 0;
}

static void __exit device_exit(void)
{
  pr_info("%s\r\n", __func__);
}

/* Register the names of custom entry and exit routines */
module_init(device_init);
module_exit(device_exit);

/* Comments which are retained in code */
MODULE_AUTHOR("debmalyasarkar1@gmail.com");
MODULE_DESCRIPTION("Shared Workqueue");
MODULE_LICENSE("GPL");

