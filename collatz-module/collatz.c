#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>

static LIST_HEAD(sequence);

struct collatz_number
{
    int number;
    struct list_head list;
};

static int start = 25;

static int __init proc_init(void)
{
    printk(KERN_INFO "Loading Kernel Module\n");

    struct collatz_number *c_number;

    int n = start;
    while (n > 1)
    {
        c_number = kmalloc(sizeof(struct collatz_number), GFP_KERNEL);
        if (c_number == NULL)
            return -ENOMEM;

        c_number->number = n;
        INIT_LIST_HEAD(&c_number->list);
        list_add_tail(&c_number->list, &sequence);

        if (n % 2 == 0)
            n /= 2;
        else
            n = 3 * n + 1;
    }

    c_number = kmalloc(sizeof(struct collatz_number), GFP_KERNEL);
    if (c_number == NULL)
        return -ENOMEM;

    c_number->number = n;
    INIT_LIST_HEAD(&c_number->list);
    list_add_tail(&c_number->list, &sequence);

    int i = 0;
    struct collatz_number *num;
    list_for_each_entry(num, &sequence, list)
    {
        printk("sequence[%d]=%d\n", i++, num->number);
    }

    return 0;
}

static void __exit proc_exit(void)
{
    printk(KERN_INFO "Removing Kernel Module\n");

    struct collatz_number *num, *next;
    list_for_each_entry_safe(num, next, &sequence, list)
    {
        list_del(&num->list);
        kfree(num);
    }
}

module_init(proc_init);
module_exit(proc_exit);

module_param(start, int, 0);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Collatz Conjecture Module");
MODULE_AUTHOR("ME");