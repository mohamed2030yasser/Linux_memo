#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/fs.h> // for registration as a file (manually)
#include <linux/cdev.h> 


/**********************************************************************/
/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("M Karem");
MODULE_DESCRIPTION("A hello world Psuedo device driver");

/**********************************************************************/
int cnt = 1;
module_param(cnt, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(cnt, "An integer");

int Major_Num=0;

int MJ_NUM = 1;
module_param(MJ_NUM, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(MJ_NUM, "The Major Number");

dev_t Dev_Num ;
struct cdev chr_dev_st ;
struct class *my_class ;
struct device* my_device;

/**** read ****/
#define RB_SIZE 15
static unsigned char RW_Buffer[RB_SIZE]= "";

/**********************************************************************/
static int Driver_open (struct inode *device_file , struct file *instance)
{
    printk("%s was called\n",__func__);
    return 0;
}

/****************/
static int Driver_close (struct inode *device_file , struct file *instance)
{
    printk("%s was called\n",__func__);
    return 0;
}

/****************/
// Test : cat /dev/test_file
static ssize_t Driver_read (struct file *File_ops, char __user *user_buffer, size_t count, loff_t * offs)
{
    int not_copied;

    /* Get amount of data to copy */
    // to_copy = min(count,buffer_pointer);
    printk("%s : the count to read %ld \n",__func__,count);
    printk("%s : the offset %lld \n",__func__,*offs);
    //*offset=0;

    if((count + *offs) > RB_SIZE)
    {
        count = RB_SIZE - *offs ; //adjust count
    }
    not_copied = copy_to_user(user_buffer,&RW_Buffer[*offs],count);
    if (not_copied)
    {
        return -1;
    }
    *offs = count;
    printk("%s : not copied %d \n",__func__,not_copied);
    printk("%s : message: %s \n",__func__, user_buffer);

    // printk("%s was called\n",__func__);
    return count;
}

/****************/
// Test : echo "Hello" > /dev/test_file
static ssize_t Driver_write (struct file *File_ops, const char __user *user_buff, size_t count, loff_t *offs)
{
    // 1-Adjust Count
    // 2-Copy from User to kernel
    // 3-Adjust offset
    // 4-return successfully count

    int not_copied;

    /* Get amount of data to copy */
    // to_copy = min(count,buffer_pointer);
    printk("%s : the count to write %ld \n",__func__,count);
    printk("%s : the offset %lld \n",__func__,*offs);
    //*offset=0;

    if((count + *offs) > RB_SIZE)
    {
        count = RB_SIZE - *offs ; //adjust count
    }
    if(!count)
    {
        printk("No Space left\n");
        return -1;
    }
    not_copied = copy_from_user(&RW_Buffer[*offs],user_buff,count);
    if (not_copied)
    {
        return -1;
    }
    *offs = count;
    printk("%s : Already Done %ld \n",__func__,count);
    printk("%s : message: %s \n",__func__, RW_Buffer);

    // printk("%s was called\n",__func__);
    return count;
}
/**********************************************************************/
struct file_operations  fops =
{
    .owner = THIS_MODULE,
    .open = Driver_open,
    .release = Driver_close,
    .read = Driver_read,
    .write = Driver_write

};
/**********************************************************************/

static int __init hello_driver_init(void)
{
    int Ret_val ;
    //Major_Num = MJ_NUM ;
    int i = 0;

    for (i = 0; i < cnt; i++)
        printk("hello module\n");

    // 1-Allocate device number
    //register_chrdev(Major_Num,"Hello_Driver",&fops); // Manually registration
    Ret_val = alloc_chrdev_region(&Dev_Num , 0 , 1 , "Hello_Driver"); // Automatically/Dynamically registration
    if (Ret_val == 0)
    {
        printk("%s Ret_val=0 , Registered Device : Major Num = %d , Minor Num = %d\n",__func__, MAJOR(Dev_Num),MINOR(Dev_Num));
    }
    else
    {
        printk("Could not register Device Number\n");
        return -1;
    }

    // 2-Define the driver (Character / Block / Network) 
    cdev_init(&chr_dev_st , &fops); // Make it ready for cdev_add
    Ret_val = cdev_add(&chr_dev_st ,Dev_Num ,1 );
    if (Ret_val != 0)
    {
        printk("Could not register Device Number\n");
        goto CHARACTER_ERROR;
    }

    // 3-Generate file (class / device file)
    if ((my_class = class_create (THIS_MODULE,"test_myclass"))==NULL)
    {
        printk("Could not create Device Class\n");
        goto CLASS_ERROR;
    }
    my_device = device_create(my_class,NULL,Dev_Num,NULL,"test_file");
    if (my_device == NULL)
    {
        printk("Could not create Device \n");
        goto DEVICE_ERROR;
    }
    printk("Hello Device Driver is created \n");
    return 0;
    /*******************************************************************/
    DEVICE_ERROR:
        class_destroy(my_class); // delete the class
    
    CLASS_ERROR:
        cdev_del(&chr_dev_st);

    CHARACTER_ERROR:
        unregister_chrdev_region(Dev_Num,1);
    
    return -1;

}
static void __exit hello_driver_exit(void)
{
    int i;

    for (i = 0; i < cnt; i++)
        printk("good bye module\n");
    
    //unregister_chrdev(Major_Num,"Hello_Driver"); // Manually 

    cdev_del(&chr_dev_st); 
    device_destroy(my_class,Dev_Num); // This func takes the class so we delete the device before class
    class_destroy(my_class); // delete the class
    unregister_chrdev_region (Dev_Num,1);  // Automatically/Dynamically
        
}
/**********************************************************************/
module_init(hello_driver_init);
module_exit(hello_driver_exit);

/**********************************************************************/
// lsmod
// insmod # modprobe
// rmmod
// modinf

/**********************************************************************/
// sudo dmesg -c --> Remove kernel messages
// make --> appear hello.ko
// sudo insmod hello.ko
// dmesg --> [17442.353682] hello module
// lsmod |head -n 5  --- > mudule is running
// sudo rmmod hello
// dmesg --> [17459.856485] good bye module

// ## For RPI ##
// sudo apt-get install raspberrypi-kernel-headers


/**********************************************************************/