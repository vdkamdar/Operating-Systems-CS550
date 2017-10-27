 #include <linux/kernel.h>
 #include <linux/module.h>
 #include <linux/kprobes.h>
 #include <linux/ktime.h>
 #include <linux/limits.h>
 #include <linux/sched.h>
 #include <linux/memory.h>
 #include <linux/memcontrol.h>
 #include <linux/proc_fs.h>
 #include <asm/uaccess.h>
 #include <linux/slab.h>


	#define BUFFER_SIZE	500
	#define PROC_NAME "Probe"

	int position=0;
	int numberOfTimesDeviceWasOpened=0;
	pid_t process_id= 1;
	module_param(process_id, int, 0);
	int dataBufferCounter = 0;

	struct DataBuffer
	{
		unsigned long address;
		long time;
	};

	struct DataBuffer buffer[BUFFER_SIZE];

	static struct jprobe my_jprobe = {
		.entry                  = handle_mm_fault,
		.kp = {
		        .symbol_name    = "handle_mm_fault",
		},
  };

static int my_open(struct inode *inod,struct file *fil);
static int my_read(struct file *filp,struct DataBuffer *out,size_t count,loff_t *offp );
static int my_release(struct inode *inod,struct file *fil);

struct file_operations proc_fops = {
	.read= my_read,
	.open = my_open,
	.release = my_release
};

static int my_read(struct file *filp,struct DataBuffer *out,size_t count,loff_t *offp )
{
	int ret=0,i=0;
	printk("Read:\n");

	for(i=0;i<BUFFER_SIZE;i++)
	{
		ret = copy_to_user(&out[i],&buffer[i],sizeof(buffer[i]));

		if(ret == 0)

		if(ret<0)
		{
			printk(KERN_ALERT "Copy!");
			ret = -1;
		}
	}
	return ret;
}

static int my_open(struct inode *inod,struct file *fil)
{
	numberOfTimesDeviceWasOpened++;
	printk(KERN_ALERT "Device opened %d times\n ",numberOfTimesDeviceWasOpened);
	return 0;
}

static int my_release(struct inode *inod,struct file *fil)
{
	numberOfTimesDeviceWasOpened--;
	printk(KERN_ALERT "Device Released %d times\n",numberOfTimesDeviceWasOpened);
	return 0;
}

static int __init jprobe_init(void)
{
	int ret;
	struct proc_dir_entry *proc_file_entry;
	ret = register_jprobe(&my_jprobe);

	if (ret == 0)

	if (ret < 0)
	{
	      printk(KERN_INFO "when jprobe registers returns %d\n", ret);
	      return -1;
  	}

	proc_file_entry = proc_create(PROC_NAME,0,NULL,&proc_fops);
	if(proc_file_entry==NULL)
	{
		printk(KERN_INFO "Proc failed to enter!\n");
	  	return -1;
	}
  printk(KERN_INFO "Jprobe is planted at %p, Handler address is %p and probe is registered Succesfully\n",my_jprobe.kp.addr, my_jprobe.entry);
  return 0;
}

static void __exit jprobe_exit(void)
{
  unregister_jprobe(&my_jprobe);
  remove_proc_entry(PROC_NAME,NULL);
  printk(KERN_INFO "jprobe at %p unregistered and probe unregistered successfully!\n", my_jprobe.kp.addr);
}

 int handle_mm_fault(struct mm_struct *mm, struct vm_area_struct *vma,unsigned long address, unsigned int flags)
 {
  struct timespec xtime;

	if(process_id == current->pid)
	{
		if(position%BUFFER_SIZE==0)
		{
			position=0;
		}
		xtime = current_kernel_time();
		buffer[position].address = address;
		buffer[position].time = xtime.tv_nsec;
		printk(KERN_INFO "Page fault for process id %d occured at Address %ld at time %ld\n",  current->pid,buffer[position].address,buffer[position].time);
			printk(KERN_INFO "Data Buffer Value %d\n",dataBufferCounter);
		position++;
		dataBufferCounter++;
	}

	jprobe_return();

	return 0;
 }
 module_init(jprobe_init)
 module_exit(jprobe_exit)
 MODULE_LICENSE("GPL");

