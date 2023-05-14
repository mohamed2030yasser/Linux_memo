#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/fs.h> // for registration as a file (manually)


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


/**********************************************************************/
static int Driver_open (struct inode *device_file , struct file *instance)
{
    printk("%s was called\n",__FUNCTION__ ));
    return0;
}

static int Driver_close (struct inode *device_file , struct file *instance)
{
    printk("%s was called\n",__FUNCTION__ ));
    return0;
}


/**********************************************************************/
struct file_operations  fops =
{
    .owner = THIS_MODULE ,
    .open = Driver_open ,
    .release = Driver_close
};
/**********************************************************************/

static int __init hellodriver_init(void)
{
    int Ret_val ;
    Major_Num = MJ_NUM ;
    int i = 0;

    for (i = 0; i < cnt; i++)
        printk("hello module\n");

    //register_chrdev(Major_Num,"Hello_Driver",&fops); // Manually registration
    
    Ret_val = register_chrdev(Major_Num,"Hello_Driver",&fops);
    if (Ret_val == 0)
    {
        printk("%s Ret_val=0 , Registered Device  ---> Done\n",__FUNCTION__ , Major_Num,0);
    }
    else
    {
        printk("Could not register Device Number\n");
        return -1;
    }
    
    return 0;
}
static void __exit hellodriver_exit(void)
{
    int i;

    for (i = 0; i < cnt; i++)
        printk("good bye module\n");
    
    unregister_chrdev(Major_Num,"Hello_Driver"); // Manually registration
        
}
module_init(hellodriver_init);
module_exit(hellodriver_exit);

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
// sudo rmmod hello
// dmesg --> [17459.856485] good bye module

// ## For RPI ##
// sudo apt-get install raspberrypi-kernel-headers

/**********************************************************************/