#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/types.h>

struct color
{
    int red;
    int green;
    int blue;
    struct list_head list;
};

static LIST_HEAD(color_list);

static struct color *color_init(int red, int green, int blue);

static int __init proc_init(void)
{
    printk(KERN_INFO "Loading Kernel Module\n");

    struct color *red = color_init(255, 0, 0);
    if (red == NULL) return -ENOMEM;

    struct color *green = color_init(0, 255, 0);
    if (green == NULL) return -ENOMEM;

    struct color *blue = color_init(0, 0, 255);
    if (blue == NULL) return -ENOMEM;

    struct color *white = color_init(255, 255, 255);
    if (white == NULL) return -ENOMEM;

    list_add_tail(&red->list, &color_list);
    list_add_tail(&green->list, &color_list);
    list_add_tail(&blue->list, &color_list);
    list_add_tail(&white->list, &color_list);

    struct color *ptr;
    list_for_each_entry(ptr, &color_list, list)
    {
        printk("color(r=%d, g=%d, b=%d)\n", ptr->red, ptr->green, ptr->blue);
    }

    return 0;
}

static void __exit proc_exit(void)
{
    printk(KERN_INFO "Removing Kernel Module\n");

    struct color *ptr, *next;
    list_for_each_entry_safe(ptr, next, &color_list, list)
    {
        printk("removing color(r=%d, g=%d, b=%d)\n", ptr->red, ptr->green, ptr->blue);
        list_del(&ptr->list);
        kfree(ptr);
    }
}

static struct color *color_init(int red, int green, int blue)
{
    struct color *c = kmalloc(sizeof(struct color), GFP_KERNEL);
    if (c == NULL)
        return NULL;
    
    c->red = red;
    c->green = green;
    c->blue = blue;
    INIT_LIST_HEAD(&c->list);

    return c;
}


module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PID Module");
MODULE_AUTHOR("ME");