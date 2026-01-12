#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>

#define BUFF_SIZE 1024
#define PROC_NAME "pid"
#define PROC_PERMS 0666

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);
ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);

/* virtual pid of the task */
static long vpid;

static const struct proc_ops proc_ops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
};

/* This function is called when the module is loaded. */
int proc_init(void)
{
    printk(KERN_INFO "Loading Kernel Module\n");

    /* creates the /proc/pid entry */
    proc_create(PROC_NAME, PROC_PERMS, NULL, &proc_ops);

    return 0;
}

/* This function is called when the module os removed. */
void proc_exit(void)
{
    printk(KERN_INFO "Removing Kernel Module\n");

    /* removes the /proc/pid entry */
    remove_proc_entry(PROC_NAME, NULL);
}

/* This function is called each time the /proc/pid is read */
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
    char buff[BUFF_SIZE];

    static int completed = 0;
    if (completed)
    {
        completed = 0;
        return 0;
    }

    completed = 1;

    struct pid *task_pid = find_vpid(vpid);
    if (task_pid == NULL)
    {
        ssize_t n = sprintf(buff, "NULL\n");

        /* copies kernel space buffer to user space usr_buf */
        if (copy_to_user(usr_buf, buff, n))
            return -EFAULT;

        return n;
    }

    struct task_struct *task = pid_task(task_pid, PIDTYPE_PID);
    if (task == NULL)
    {
        ssize_t n = sprintf(buff, "NULL\n");

        /* copies kernel space buffer to user space usr_buf */
        if (copy_to_user(usr_buf, buff, n))
            return -EFAULT;

        return n;
    }

    char state = task_state_to_char(task);
    ssize_t n = sprintf(buff,
                        "command=[%s] pid=[%d] state=[%c]\n",
                        task->comm,
                        task->pid,
                        state);

    /* copies kernel space buffer to user space usr_buf */
    if (copy_to_user(usr_buf, buff, n))
        return -EFAULT;

    return n;
}

/* This function is called each time when someone writes to /proc/pid */
ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos)
{
    /* allocate kernel memory */
    char *k_mem = kmalloc(count + 1, GFP_KERNEL);
    if (k_mem == NULL)
        return -ENOMEM;

    /* copies user space usr_buf to kernel memory */
    if (copy_from_user(k_mem, usr_buf, count))
        return -EFAULT;

    k_mem[count] = '\0';

    if (kstrtol(k_mem, 10, &vpid) != 0)
        printk(KERN_ERR "pid: kstrtol error\n");

    /* return kernel memory */
    kfree(k_mem);

    return count;
}

/* Macros for registering module entry and exit points. */
module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PID Module");
MODULE_AUTHOR("ME");