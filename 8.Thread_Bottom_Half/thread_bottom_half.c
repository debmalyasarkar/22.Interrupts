/* A kernel thread is set up which contains the bottom half code */
/* The thread is made to wait unless the ISR wakes its up */
/* When ISR wakes up thread, the bottom half tasks are performed */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/wait.h>

#define SHARED_IRQ 1

static int irq = SHARED_IRQ, dev_id, irq_counter = 0;

static DECLARE_WAIT_QUEUE_HEAD(wq);

static atomic_t cond;

static struct task_struct *task;

module_param(irq, int, S_IRUGO);

static irqreturn_t my_interrupt(int irq, void *dev_id)
{
  irq_counter++;
  pr_info("In ISR Counter = %d\r\n",irq_counter);

  /* set atomic counter and wake up thread (BH) */
  atomic_set(&cond, 1);
  
  wake_up_interruptible(&wq);
  
  return IRQ_NONE;
}

static int t_fun(void *t_arg)
{
  do
  {
    atomic_set(&cond, 0);
    /* Wait until either thread is suspended or an atomic counter is set */
    wait_event_interruptible(wq, kthread_should_stop() || atomic_read(&cond));
    pr_info("Entering %s , Context Of %s, PID %d\r\n", __func__, current->comm, current->pid);
  }
  while(!kthread_should_stop());
  return 0;
}


static int __init device_init(void)
{
  if(request_irq(irq, my_interrupt,IRQF_SHARED, "my_interrupt", &dev_id))
    return -1;
  /* Create and wake up a thread */
  task = kthread_run(t_fun, NULL, "mykthread");
  return 0;
}

static void __exit device_exit(void)
{
  synchronize_irq(irq);
  free_irq(irq, &dev_id);
  /* Stop a thread */
  kthread_stop(task);
}

/* Register the names of custom entry and exit routines */
module_init(device_init);
module_exit(device_exit);

/* Comments which are retained in code */
MODULE_AUTHOR("debmalyasarkar1@gmail.com");
MODULE_DESCRIPTION("Thread as Bottom Half");
MODULE_LICENSE("GPL");

