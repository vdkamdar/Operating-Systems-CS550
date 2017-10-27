#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("GPL");
static int     minorNumber;
static char    kernel_buffer[9000] = {0};
static char    state_str[50] = {0};
static short   size_of_buffer;

static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);

static struct file_operations fops =
{
	.open = dev_open,
        .read = dev_read,
        .release = dev_release,
};

static struct miscdevice my_misc_device =
{
	.minor = MISC_DYNAMIC_MINOR,
	.name = "process_list",
	.fops = &fops
};

static struct task_struct *task;

static int __init processchar_init(void)
{
        minorNumber = misc_register(&my_misc_device);
	if (minorNumber<0)
	{
		printk(KERN_ALERT "Process_list failed to register a minor number\n");
		misc_deregister(&my_misc_device);
		return minorNumber;
	}
	printk(KERN_INFO "Process_list registered correctly with major number: 10 and minor number: %d\n", minorNumber);
	return 0;
}

static void __exit processchar_exit(void)
{
	misc_deregister(&my_misc_device);
	printk(KERN_INFO "Process_list saying Goodbye from the LKM!\n");
}

static int dev_open(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO "Process_list has been opened\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *user_buffer, size_t len, loff_t *offset)
{
	int error_count=0;

	if(error_count < 0)
	{
		return error_count;
	}

	for_each_process(task)
	{
                if (task->state == 0){sprintf(state_str, "TASK_RUNNING");}
                else if (task->state == 1){sprintf(state_str, "TASK_INTERRUPTIBLE");}
                else if (task->state == 2){sprintf(state_str, "TASK_UNINTERRUPTIBLE");}
                else if (task->state == 4){sprintf(state_str, "__TASK_STOPPED");}
                else if (task->state == 8){sprintf(state_str, "__TASK_TRACED");}
                else if (task->state == 16){sprintf(state_str, "EXIT_DEAD");}
                else if (task->state == 32){sprintf(state_str, "EXIT_ZOMBIE");}
                else if (task->state == 64){sprintf(state_str, "TASK_DEAD");}
                else if (task->state == 128){sprintf(state_str, "TASK_WAKEKILL");}
                else if (task->state == 256){sprintf(state_str, "TASK_WAKING");}
                else if (task->state == 512){sprintf(state_str, "TASK_PARKED");}
                else if (task->state == 1024){sprintf(state_str, "TASK_NOLOAD");}
                else if (task->state == 2048){sprintf(state_str, "TASK_NEW");}
                else if (task->state == 4096){sprintf(state_str, "TASK_STATE_MAX");}
		else if (task->state == (16 | 32)){sprintf(state_str, "EXIT_ZOMBIE , EXIT_DEAD");}
                else if (task->state == (128 | 2)){sprintf(state_str, "TASK_WAKEKILL , TASK_UNINTERRUPTIBLE");}
                else if (task->state == (128 | 4)){sprintf(state_str, "TASK_WAKEKILL , __TASK_STOPPED");}
                else if (task->state == (128 | 8)){sprintf(state_str, "TASK_WAKEKILL , __TASK_TRACED");}
                else if (task->state == (2 | 1024)){sprintf(state_str, "TASK_UNINTERRUPTIBLE , TASK_NOLOAD");}
                else if (task->state == (1 | 2)){sprintf(state_str, "TASK_INTERRUPTIBLE , TASK_UNINTERRUPTIBLE");}
                else if (task->state == (0 | 1 | 2 | 4 | 8 | 32 |16)){sprintf(state_str, "TASK_RUNNING , TASK_INTERRUPTIBLE , TASK_UNINTERRUPTIBLE , __TASK_STOPPED , __TASK_TRACED , EXIT_ZOMBIE , EXIT_DEAD");}
                else if (task->state == (1 | 2 | 4 | 8)){sprintf(state_str, "TASK_NORMAL , __TASK_STOPPED , __TASK_TRACED");}
                else {sprintf(state_str, "OTHER");}

		sprintf(kernel_buffer + strlen(kernel_buffer), "PID=%d  PPID=%d  CPU=%d  STATE=%s\n", task->pid, task->real_parent->pid, task_cpu(task), state_str);
		size_of_buffer = strlen(kernel_buffer);
	}
	error_count = copy_to_user(user_buffer, kernel_buffer, size_of_buffer);
	return error_count;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO "Process_list successfully closed\n");
	return 0;
}

module_init(processchar_init);
module_exit(processchar_exit);
