#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
 
#define SHARED_IRQ 1
#define SUCCESS    0

/* Device Specific Data Structure */
struct device_priv_t
{
  int dummyVar;
};

static struct device_priv_t dev_private_obj;

static int irq = SHARED_IRQ;

/* Change the irq as per system */
module_param(irq, int, S_IRUGO);

/* This routine runs in hard interrupt context and is needed for 
   1. check and validate interrupt source
   2. wake up thread context interrupt handler.
*/
static irqreturn_t my_interrupt_handler(int irq, void *dev_private_obj)
{
  static int kn_counter = 0;
  pr_info("In the Kernel ISR, Counter = %d\r\n", ++kn_counter);
  /* Ensure Interrupt is valid and handover control to thread handler */
  return IRQ_WAKE_THREAD;
}

static irqreturn_t my_threaded_handler(int irq, void *dev_private_obj)
{
  static int th_counter;
  pr_info("In the Thread ISR, Counter = %d\r\n", ++th_counter);
  return IRQ_NONE;
}

static int __init device_init(void)
{
  pr_info("%s\r\n",__func__);
  if(request_threaded_irq(irq, my_interrupt_handler, my_threaded_handler, IRQF_SHARED, "my_interrupt", &dev_private_obj))
    return -EIO;
  return SUCCESS;
}

static void __exit device_exit(void)
{
  pr_info("%s\r\n",__func__);
  /* Verify if ISR is running */
  synchronize_irq(irq);
  free_irq(irq, &dev_private_obj);
}

/* Register the names of custom entry and exit routines */
module_init(device_init);
module_exit(device_exit);

/* Comments which are retained in code */
MODULE_AUTHOR("debmalyasarkar1@gmail.com");
MODULE_DESCRIPTION("Threaded Interrupt APIs");
MODULE_LICENSE("GPL");

