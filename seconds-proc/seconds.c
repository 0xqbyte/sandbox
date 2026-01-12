#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/jiffies.h>
#include <asm/uaccess.h>
#include <asm/param.h>

#define BUFFER_SIZE 1024
#define PROC_NAME "seconds"
#define PROC_PERMS 0666

static unsigned long jiffies_on_loaded;
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

static struct proc_ops proc_ops = {
    .proc_read = proc_read,
};

/* This function is called when the module is loaded. */
int proc_init(void)
{
    /* creates the /proc/seconds entry */
    proc_create(PROC_NAME, PROC_PERMS, NULL, &proc_ops);

    /* holds jiffies value when module is loaded */
    jiffies_on_loaded = jiffies;

    return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void)
{
    /* removes the /proc/seconds entry */
    remove_proc_entry(PROC_NAME, NULL);
}

/* This function is called each time the /proc/seconds is read */
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
    int rv = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;

    if (completed)
    {
        completed = 0;
        return 0;
    }

    completed = 1;

    /* frequency of the timer interrupts since the kernel module was loaded */
    unsigned long diff = jiffies - jiffies_on_loaded;

    /* the number of elapsed seconds since the kernel module was loaded */
    unsigned long elapsed = diff / HZ;

    rv = sprintf(buffer, "%lu\n", elapsed);

    /* copies kernel space buffer to user space usr_buf */
    copy_to_user(usr_buf, buffer, rv);

    return rv;
}

/* Macros for registering module entry and exit points. */
module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds Module");
MODULE_AUTHOR("ME");