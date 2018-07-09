#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/delay.h>
 
#define SHARED_IRQ 1
#define SUCCESS    0

static int irq = SHARED_IRQ, dev_id;
/* Change the irq as per system */
module_param(irq, int, S_IRUGO);

static int delay = 0;
module_param(delay, int, S_IRUGO);

static atomic_t counter_bh, counter_th;

static void t_func(unsigned long t_arg);

/* Creating a new tasklet inside kernel with tasklet_vec, its called t_name */
static DECLARE_TASKLET(t_name, t_func, 0);

/* Bottom Half - tasklet function */
static void t_func(unsigned long t_arg)
{
  atomic_inc(&counter_bh);
  pr_info("In BH: counter_th %d, counter_bh %d, jiffies %ld\r\n", 
    atomic_read(&counter_th), atomic_read(&counter_bh), jiffies);
}

/* Top Half - ISR */
static irqreturn_t my_interrupt_handler(int irq, void * dev_id)
{
  atomic_inc(&counter_th);
  pr_info("In the ISR, jiffies = %ld\r\n", jiffies);
  tasklet_schedule(&t_name);
  mdelay(delay);
  return IRQ_NONE;
}

static int __init device_init(void)
{
  pr_info("%s\r\n",__func__);
  if(request_irq(irq, my_interrupt_handler, IRQF_SHARED, "my_interrupt", &dev_id))
    return -EIO;
  return SUCCESS;
}

static void __exit device_exit(void)
{
  pr_info("%s\r\n",__func__);
  pr_info("counter_th %d, counter_bh %d, jiffies %ld\r\n", 
    atomic_read(&counter_th), atomic_read(&counter_bh), jiffies);
  /* Verify if ISR is running */
  synchronize_irq(irq);
  free_irq(irq, &dev_id);
}

/* Register the names of custom entry and exit routines */
module_init(device_init);
module_exit(device_exit);

/* Comments which are retained in code */
MODULE_AUTHOR("debmalyasarkar1@gmail.com");
MODULE_DESCRIPTION("Print top half and bottom half execution counts");
MODULE_LICENSE("GPL");

