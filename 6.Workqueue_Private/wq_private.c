#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/sched.h>

struct workqueue_struct *myworkqueue;
struct work_struct mywork;

/* Workqueue Function - BH Routine */
static void mywork_func(struct work_struct *mywork_arg)
{
  pr_info("Current Task %s, PID %d\r\n",current->comm, current->pid);
}

static int __init device_init(void)
{
  myworkqueue = create_workqueue("my_workq");
  INIT_WORK(&mywork, mywork_func);
  queue_work(myworkqueue, &mywork);
  pr_info("%s created wq and scheduled work\r\n", __func__);
  return 0;
}

static void __exit device_exit(void)
{
  destroy_workqueue(myworkqueue);
  pr_info("%s destroyed wq\r\n", __func__);
}

/* Register the names of custom entry and exit routines */
module_init(device_init);
module_exit(device_exit);

/* Comments which are retained in code */
MODULE_AUTHOR("debmalyasarkar1@gmail.com");
MODULE_DESCRIPTION("Private Workqueue");
MODULE_LICENSE("GPL");

