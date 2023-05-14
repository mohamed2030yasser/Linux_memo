
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>


/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("memo yasser");
MODULE_DESCRIPTION("A hello world Psuedo device driver");
//==========================================

/*//==================== recive major as argument =======
int n_major = 0;
module_param(n_major,int,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(n_major,"major_number");
int major_number=0;
*/
dev_t device_num;
struct cdev st_character_device;
struct class *myclass;
struct device *mydevice;
//=====================================
#define size    15
static unsigned char buffer[size]="" ;
// echo "hi" >> /dev/test_file
ssize_t driver_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *offs)
{
    //adjust count
    //copy from user space to kernel
    //adjust offset
    //return
    int not_copied;

    printk("%s : the count to write %ld \n",__func__,count);
    printk("%s : the offset %lld \n",__func__,*offs);
    if((count + *offs) > size) //adjust count
    {
        count = size - *offs;
    }
    if(!count)
    {
        printk("no space left");
        return -1;
    }
    not_copied=copy_from_user(&buffer[*offs],user_buffer,count);
    if (not_copied)
    {
        return -1;
    }
    printk("%s : already done %ld \n",__func__,count);
    printk("%s : messege: %s \n",__func__,buffer);
    *offs=count;
    return count;
}
ssize_t driver_read(struct file *file, char __user *user_buffer, size_t count, loff_t *offs)
{
    int not_copied;

    /* Get amount of data to copy */
    // to_copy = min(count,buffer_pointer);
    printk("%s : the count to read %d \n",__func__,count);
    printk("%s : the offset %lld \n",__func__,*offs);
    if((count + *offs) > size)
    {
        count = size - *offs;
    }
    not_copied=copy_to_user(user_buffer, &buffer[*offs], count);
    if (not_copied)
    {
        return -1;
    }
    printk("%s : not_copied %d \n",__func__,not_copied);
    printk("%s : messege: %s \n",__func__,user_buffer);
    *offs=count;
    return count;
}

//==========================================
static int driver_open (struct inode *device_file, struct file *instance)
{
    printk("%s",__FUNCTION__);
    return 0;
}
static int driver_close (struct inode *device_file, struct file *instance)
{
    printk("%s",__FUNCTION__);
    return 0;
}


struct file_operations fops =
{
    .owner=THIS_MODULE,
    .open=driver_open,
    .release=driver_close,
    .read=driver_read,
    .write=driver_write
};

//============================================================
static int __init module00_init(void)
{
    int retval;
    //major_number=n_major;
    printk("hi from kernel");

    /*
    retval =register_chrdev(major_number,"hello driver",&fops);
    if(retval == 0)
    {
         printk("%s Ret_val=0 , Registered Device : Major Num = %d , Minor Num = %d\n",__FUNCTION__,major_number,0);
    }else
    {
        printk("Could not register Device Number\n");
        return -1;
    }*/
    ///////////////////// step 1 >> allocate device number
    retval = alloc_chrdev_region(&device_num,0,1,"mpu6050_device");
    if(retval == 0)
    {
         printk("%s Ret_val=0 , Registered Device : Major Num = %d , Minor Num = %d\n",__FUNCTION__,MAJOR(device_num),MINOR(device_num));
    }else
    {
        printk("Could not register Device Number\n");
        return -1;
    }

    //////////////////// step 2 >> add as char or block
    cdev_init(&st_character_device,&fops);
    retval=cdev_add(&st_character_device, device_num, 1);
    if(retval != 0)
    {
        printk("Could not register Device Number\n");
        goto CHARACTER_ERROR;
    }
    //////////////////// step 3 >> generate file (class -device) >> parameter of device
    //in /sys/class
    if((myclass = class_create(THIS_MODULE,"mpu6050_class")) == NULL)
    {
        printk("device class can not be created!\n");
        goto CLASS_ERROR;
        return -1;
    }
    mydevice=device_create(myclass, NULL, device_num,NULL, "mpu6050_test");
    if(mydevice == NULL)
    {
        printk("device class can not be created!\n");
        goto DEVICE_ERROR;
        return -1;
    }
    printk("device driver is created");
    return 0;



DEVICE_ERROR:
    class_destroy(myclass);
CLASS_ERROR:
    cdev_del(&st_character_device);
CHARACTER_ERROR:
    unregister_chrdev_region(device_num, 1);


    return -1;
}
static void __exit module00_exit(void)
{
    //unregister_chrdev(major_number,"hello driver");
    device_destroy(myclass,device_num);
    class_destroy(myclass);
    cdev_del(&st_character_device);
    unregister_chrdev_region(device_num, 1);
    printk("bye from kernel");

}
module_init(module00_init); //call when insert
module_exit(module00_exit);
//=======================================================

// sudo mknod /dev/{name} c 50 0 .......... c>> chrdev , major , manor

