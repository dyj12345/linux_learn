#include <linux/module.h>
#include <linux/moduleloader.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/elf.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/gfp.h>

#include <asm/pgtable.h>
#include <asm/sections.h>
#include <asm/unwind.h>

#include <linux/init.h>
#include <asm/io.h>//arch/arm/include/asm/io.h
#include <linux/ioport.h>

#include <linux/types.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/device.h>

#define key_paddr 0xE0200C40
#define led_paddr 0xE0200280

unsigned long *key_ctrl_vaddr=NULL;
unsigned long *key_data_vaddr=NULL;
unsigned long *led_ctrl_vaddr=NULL;
unsigned long *led_data_vaddr=NULL;

static unsigned int this_major = 250;
static unsigned int this_minor = 0;

struct class *key_led_class;

static int key_led_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long args)
{
	unsigned int val;
	
	switch(cmd)
	{
		printk("%d-%d\n",cmd,args);
		//led on
		case 0:
		   	val = ioread32(led_data_vaddr);
			val &=~(0x1<<args);
			
			iowrite32(val,led_data_vaddr);
			break;
		case 1:
			val = ioread32(led_data_vaddr);
			val |=(0x1<<args);
			iowrite32(val,led_data_vaddr);
			break;
		default:
			printk("cmd is wrong, 0-->LED OFF  1-->LED ON \n");	
			return -EINVAL;
	}
	return 0;
}
static int key_led_open (struct inode *inode, struct file *file)
{
	printk("open!\n");
	return 0;
}
static int key_led_release (struct inode *inode, struct file *file)
{
	printk("release!\n");
	return 0;
}
struct file_operations key_led_fops={
	.owner = THIS_MODULE,
	.open = key_led_open,
	.release = key_led_release,
	.ioctl = key_led_ioctl,
};

static __init int key_led_init(void)
{
	printk("key_led_init!\n");
	int result;
	result = register_chrdev(this_major, "key_led", &key_led_fops);
	if(result<0)
	{
		printk("register_chrdev error!\n");
		goto register_chrdev_error;
	}
	key_led_class = class_create(THIS_MODULE, "key_led");
	if(IS_ERR(key_led_class))
	{
		printk("Err:failed in creating class.\n");
		goto class_create_error;
	}	
	if(device_create(key_led_class,NULL,MKDEV(this_major, this_minor), NULL, "key_led")==NULL)
	{
		printk("device_create_error!\n");
		goto device_create_error;
	}
	
	struct resource *resource;
	resource=request_mem_region(key_paddr, 8, "key_men");
	if(resource==NULL)
	{
		printk("request_mem_region_error!\n");
		goto request_mem_region;
	}
	resource=request_mem_region(led_paddr, 8, "led men");
	if(resource==NULL)
	{
		printk("request_mem_region_error!\n");
		goto request_mem_region;
	}

	key_ctrl_vaddr = ioremap(key_paddr, 8);
	key_data_vaddr = key_ctrl_vaddr+1;
	
	led_ctrl_vaddr = ioremap(led_paddr, 8);
	led_data_vaddr = led_ctrl_vaddr+1;

	if(key_ctrl_vaddr==NULL ||led_ctrl_vaddr ==NULL)
	{
		printk("ioremap_error!\n");
		goto ioremap_error;
	}
	//key_ctrl,input method
	unsigned int val;
	val = ioread32(key_ctrl_vaddr);
	val &= ~(0xf<<0);
	iowrite32(val,key_ctrl_vaddr);

	//led_ctrl,output method
	val = ioread32(led_ctrl_vaddr);
	val &=~(0xf<<0);
	val |= (0x1<<0);
	iowrite32(val,led_ctrl_vaddr);

	//led_data
	val = ioread32(led_data_vaddr);
	val |=(0x1<<0);
	iowrite32(val,led_data_vaddr);
	
	return 0;

	
ioremap_error:
	release_mem_region(key_paddr, 8);
	release_mem_region(led_paddr, 8);
request_mem_region:
	device_destroy(key_led_class, MKDEV(this_major, this_minor));
device_create_error:
	class_destroy(key_led_class);
class_create_error:
	unregister_chrdev(this_major,"key_led");
register_chrdev_error:
	return -1;
}
static __exit void key_led_exit(void)
{
	printk("key_led_exit!\n");
	unsigned int val;
	val = ioread32(led_data_vaddr);
	val |= (0x1<<0);
	iowrite32(val,led_data_vaddr);

	iounmap(key_ctrl_vaddr);
	iounmap(led_ctrl_vaddr);

	release_mem_region(key_paddr, 8);
	release_mem_region(led_paddr, 8);

	device_destroy(key_led_class, MKDEV(this_major, this_minor));
	class_destroy(key_led_class);

	unregister_chrdev(this_major,"key_led");
}

module_init(key_led_init);
module_exit(key_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Achilles");


