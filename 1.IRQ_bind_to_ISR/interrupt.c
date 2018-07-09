#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>

#define SHARED_IRQ 1
#define SUCCESS    0

/* Device Specific Data Structure */
struct device_priv_t
{
  int dummyVar;
};

static struct device_priv_t dev_private_obj;

static int irq = SHARED_IRQ, irq_counter = 0;

/* In different machines, different irq lines are used for keyboard */
/* So we dynamically take IRQ as argument to this module */
/* Usage : $insmod interrupt.ko irq=<IRQNO> */
module_param(irq, int, S_IRUGO);

static irqreturn_t my_interrupt_handler(int irq, void * dev_private_obj)
{
  irq_counter++;
  pr_info("In the ISR, Counter = %d\r\n",irq_counter);
  return IRQ_NONE;
}

static int __init device_init(void)
{
  pr_info("%s\r\n",__func__);
  if(request_irq(irq, my_interrupt_handler, IRQF_SHARED, "my_interrupt", &dev_private_obj))
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
MODULE_DESCRIPTION("Request IRQ line and bind ISR to the IRQ Line");
MODULE_LICENSE("GPL");
