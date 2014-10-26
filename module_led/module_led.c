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

#include <asm/io.h>//arch/arm/include/asm/io.h
#include <linux/ioport.h>

#define LED_CTRL_addr_P 0xE0200280

static unsigned int led_major = 0;
static unsigned int led_minor = 0;

static long *led_ctrl_addr_v;
static long *led_data_addr_v;

int led_open(struct inode *inode, struct file *file)
{
	printk("led_open!\n");
	
	
	return 0;
}
void led_release(struct inode *inode, struct file *file)
{
	printk("led_release!\n");

}
int led_ioctl(struct inode *inode, struct file *file, unsigned int num, unsigned long args)
{
	return 0;
}
static struct file_operations led_fops={
	.owner = THIS_MODULE,
	.open = led_open,
	.release = led_release,
	.ioctl = led_ioctl,

};

static __init int led_init()
{
	printk("led_hi!\n");
	int result;
	result = register_chrdev(led_major, "led", &led_fops);

	//申请内存
	result = request_mem_region(LED_CTRL_addr_P, 8, "led");

	//内存映射
	led_ctrl_addr_v = ioremap(LED_CTRL_addr_P, 8);
	led_data_addr_v = led_ctrl_addr_v+1;

	//输出模式
/*	*led_ctrl_addr_v &= ~(0xf<<0);
	*led_ctrl_addr_v |= (0x1<<0);

	//开灯
	*led_data_addr_v &= ~(0x1<<0);
*/
	unsigned int val;
	val = ioread32(led_ctrl_addr_v);
	val &= ~(0xf<<0);
	val |= (0x1<<0);
	iowrite32(val,led_ctrl_addr_v);

	
	val = ioread32(led_data_addr_v);
	val &= ~(0x1<<0);
	iowrite32(val,led_data_addr_v);
	

	return 0;
}
static __exit void led_exit()
{
	printk("led_bye!\n");
	//清零
//	*led_data_addr_v |= (0x1<<0);

	unsigned int val;
	val = ioread32(led_data_addr_v);
	val |= (0x1<<0);
	iowrite32(val,led_data_addr_v);

	iounmap(led_ctrl_addr_v);
	release_mem_region(LED_CTRL_addr_P,8);
	
	unregister_chrdev(led_major, "led");
	
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");

