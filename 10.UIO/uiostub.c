#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/uio_driver.h>
 
#define SUCCESS 0

static struct uio_info *uio_dev;
static struct device   *parent_dev;

static int irq = 1;
/* Set irq dynamically based on system IRQ configuration */
module_param(irq, int, S_IRUGO);

static void my_release(struct device *dev)
{
  pr_info("Releasing my_uio_device\r\n");
}

static irqreturn_t my_handler(int irq, struct uio_info *info)
{
  static int irq_counter = 0;
  pr_info("In the ISR, Counter = %d\r\n",++irq_counter);
  /* Must return IRQ_HANDLED for events to reach userspace */
  /* If IRQ_NONE returned then ISR executes but events do not reach userspace */ 
  return IRQ_HANDLED;
}
 
static int __init device_init(void)
{
  pr_info("%s\r\n",__func__);

  /* Setup and Register Parent Device */
  parent_dev = kzalloc(sizeof(struct device), GFP_KERNEL);
  dev_set_name(parent_dev, "my_device");
  parent_dev->release = my_release;
  device_register(parent_dev);

  /* Setup IRQ Information */
  uio_dev = kzalloc(sizeof(struct uio_info), GFP_KERNEL);
  uio_dev->name = "my_uio_device";
  uio_dev->version = "0.0.1";
  uio_dev->irq = irq;
  uio_dev->irq_flags = IRQF_SHARED; 
  uio_dev->handler =  my_handler;
  
  /* Register UIO Device */
  if(uio_register_device(parent_dev, uio_dev) < 0)
  {
    device_unregister(parent_dev);
    kfree(parent_dev);
    kfree(uio_dev);
    pr_info("Failing to Register UIO Device\r\n");
    return -EIO;
  }
  pr_info("Registered UIO Handler for IRQ = %d\r\n",irq);
  return SUCCESS;
}

static void __exit device_exit(void)
{
  pr_info("%s\r\n",__func__);
  uio_unregister_device(uio_dev);
  device_unregister(parent_dev);
  kfree(parent_dev);
  kfree(uio_dev);
  pr_info("Unregistered UIO Handler for IRQ = %d\r\n",irq);
}

/* Register the names of custom entry and exit routines */
module_init(device_init);
module_exit(device_exit);

/* Comments which are retained in code */
MODULE_AUTHOR("debmalyasarkar1@gmail.com");
MODULE_DESCRIPTION("Setup UIO to Access Events from User Space App");
MODULE_LICENSE("GPL");

