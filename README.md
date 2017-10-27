# Operating-Systems-CS550

**Technologies: Google Cloud Platform, Linux, C**

**Program1**

- Recursively create a process hirearchy tree, H levels deep, print out the process tree information as the tree is being created, and then systematically terminate all the processes in the tree.

- Using system calls: fork(), exec(), wait(), getpid(), getppid()

**Program2**

- A Kernel module to create a character device supporting read operation.

- When read system call is invoked on the character device from the user space, it returns the following information:
   
     1. process ID
     2. parent process ID
     3. CPU on which the process is running
     4. current state
     
 **Program3**
    
- Kernel level pipe for exchanging lines between producers and consumers using semaphore based solution.

-	Miscellaneous Character device is used as a kernel module to maintain wait queues and uses concepts of concurrency and synchronization. 

**Program4**

- Used to probe a process using the process ID.

-Underlying implementation using jprobe
