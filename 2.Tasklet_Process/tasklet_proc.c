#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/sched.h>

static void t_func(unsigned long t_arg);

/* Private data structure for tasklet */
struct t_struct
{
  int a;
  int b;
};

static struct t_struct t_data;

/* Creating a new tasklet inside kernel with tasklet_vec, its called t_name */
static DECLARE_TASKLET(t_name, t_func, (unsigned long)&t_data);

/* Bottom Half - tasklet function */
static void t_func(unsigned long t_arg)
{
  struct t_struct *ptr;
  ptr = (struct t_struct *)t_arg;

  pr_info("Entering t_func, p->a = %d, jiffies = %ld\r\n", ptr->a, jiffies);
  /* Prints the context from where tasklet is invoked */
  pr_info("Entering t_func, in context of %s, with pid = %d\r\n",current->comm, current->pid);
}

static int __init device_init(void)
{
  t_data.a = 100;
  t_data.b = 200;
  pr_info("Loading Module\r\n");
  pr_info("Scheduling tasklet, jiffies= %ld\r\n",jiffies);
  /* Scheduling tasklet with tasklet_vec kernel queue */
  tasklet_schedule(&t_name);
  return 0;
}

static void __exit device_exit(void)
{
  pr_info("Unloading Module\r\n");
}

/* Register the names of custom entry and exit routines */
module_init(device_init);
module_exit(device_exit);

/* Comments which are retained in code */
MODULE_AUTHOR("debmalyasarkar1@gmail.com");
MODULE_DESCRIPTION("Create and run a new tasklet from process context");
MODULE_LICENSE("GPL");

