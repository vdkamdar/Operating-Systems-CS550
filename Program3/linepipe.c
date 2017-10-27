#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/rtc.h>
#include <linux/sched.h>
MODULE_LICENSE("GPL");

static int minorNumber;
static struct miscdevice linepipe;
static int N;
static int char_count = 100;
static int read_index = 0, write_index = 0;
static int pipe_empty_slots;
static struct semaphore full;
static struct semaphore empty;
static struct semaphore read_op_mutex;
static struct semaphore write_op_mutex;
char** pipe;

module_param(N, int, 0000);

/*declaring fops functions*/
static int dev_open(struct inode*, struct file*);
static ssize_t dev_read(struct file*, char*, size_t, loff_t*);
static ssize_t dev_write(struct file*, const char*, size_t, loff_t*);
static int dev_release(struct inode*, struct file*);

/*fops struct*/
static struct file_operations fops = {
        .open = &dev_open,
        .read = &dev_read,
        .write = &dev_write,
        .release = &dev_release
};

//initialize module
int init_module(){

        linepipe.name = "linepipe";
        linepipe.minor = MISC_DYNAMIC_MINOR;
	linepipe.fops = &fops;

        minorNumber = misc_register(&linepipe);
        if(minorNumber < 0){
                printk(KERN_ERR "Could not register the device\n");
				misc_deregister(&linepipe);
                return minorNumber;
        }

        printk(KERN_INFO "Device linepipe registered with Number %d\n", N);
        int count = 0;
        pipe = (char**)kmalloc(N*sizeof(char*), GFP_KERNEL);
        while(count < N){
                pipe[count] = (char*)kmalloc((char_count+1)*sizeof(char), GFP_KERNEL);
                pipe[char_count] = '\0';
                ++count;
        }

        //Initializing semaphores and mutexes
        sema_init(&full, 0);
	sema_init(&empty, N);
        sema_init(&read_op_mutex, 1);
        sema_init(&write_op_mutex, 1);

        //Initializing pipe
        pipe_empty_slots = N;
        return 0;

}

//This function will be called when the device is opened
static int dev_open(struct inode* inodep, struct file* filep){
        printk(KERN_INFO "Device opened\n");
        return 0;
}

//This function will be called when reading pipe
static ssize_t dev_read(struct file* filep, char* buffer, size_t len, loff_t* offset){

	//Perform down operation when lock is accquired
        down_interruptible(&read_op_mutex);
        down_interruptible(&full);
	read_index %= N;
	int i;

        for(i = 0; i < len; i++){
                if(N <= pipe_empty_slots){
                        break;
                }
                copy_to_user(&buffer[i], &pipe[read_index][i], 1);
        }
        ++read_index;
        ++pipe_empty_slots;

	//Perform up operation after completing the operation
        up(&empty);
        up(&read_op_mutex);
        return i;
}

/*function called when device is written to*/
static ssize_t dev_write(struct file* filep, const char* buffer, size_t len, loff_t* offset){

        int i = 0;
        //Perform down operation when lock is accquired
        down_interruptible(&write_op_mutex);
        down_interruptible(&empty);
		        write_index %= N;
        for(i = 0; i < len; i++){
                if(pipe_empty_slots <= 0){
                        break;
                }
                copy_from_user(&pipe[write_index][i], &buffer[i], 1);
        }
        ++write_index;
        --pipe_empty_slots;

	//Perform up operation after completing the operation
        up(&full);
        up(&write_op_mutex);
        return i;
}

/*function that is called when device is closed*/
static int dev_release(struct inode* inodep, struct file* filep){
        /*decrementing the number of open devices*/
        printk(KERN_INFO "Linepipe device closed\n");
        return 0;
}

/*function to cleanup the module*/
void cleanup_module(){
        int i;
        for(i = 0; i < N; i++){
                kfree(pipe[i]);
        }
        misc_deregister(&linepipe);
        printk(KERN_INFO "Linepipe device unregistered!\n");
}
