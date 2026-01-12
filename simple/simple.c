#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/jiffies.h>
#include <asm/param.h>

static unsigned long jiffies_diff;

/* This function is called when the module is loaded. */
int simple_init(void)
{
    printk(KERN_INFO "Loading Kernel Module\n");

    /*
        HZ determines the frequency of the timer interrupt,
        for example HZ=100, a timer interrupt occurs 100 times per
        second, or every 10 milliseconds
    */
    printk(KERN_INFO "HZ=%d\n", HZ);

    /*
        global variable jiffies, which maintains the number
        of timer interrupts that have occurred since the
        system was booted
    */
    printk(KERN_INFO "jiffies=%lu\n", jiffies);

    /* holds jiffies value when module is loaded */
    jiffies_diff = jiffies;

    return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void)
{
    printk(KERN_INFO "Removing Kernel Module\n");
    printk(KERN_INFO "jiffies=%lu\n", jiffies);

    /* frequency of timer interrupts between module loading and removing */
    jiffies_diff = jiffies - jiffies_diff;

    printk(KERN_INFO "jiffies_diff=%lu\n", jiffies_diff);

    /*
        the number of seconds that have elapsed
        since the time the kernel module was loaded and then removed
    */
    unsigned long module_active_time = jiffies_diff / HZ;

    printk(KERN_INFO "module_active_time=%lu\n", module_active_time);
}

/* Macros for registering module entry and exit points. */
module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("ME");