#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include "enc_fb.h"
#define ENC_FB_MAJOR 323
#define ENC_FB_MINOR 1024
#define DEVICE_NAME "enc_fb"

static DEFINE_MUTEX(enc_devices_mtx);
static DEFINE_MUTEX(screen_info_mtx);
static DEFINE_MUTEX(enc_info_mtx);
static DEFINE_MUTEX(enc_data_mtx);

static HLIST_HEAD(enc_devices);

struct enc_fb_context{
    struct enc_screen_info screenif;
    struct enc_info encif;
    struct enc_data encdt;
};
struct enc_fb_device{
    struct enc_fb_context enc_context;
    struct hlist_node hlist;
    struct cdev cdev;
    struct device class_dev;
    char* name;
};
struct class* enc_fb_class;

static int enc_devices_param = 1;
module_param_named(num_devices, enc_devices_param, int, 0444);


static int init_context(struct enc_fb_context* context){
    if(!context)
	return -1;
//    context->screenif = kzalloc(sizeof(*(context->screenif)), GFP_KERNEL);
//    if(!context->screenif)
//        return -ENOMEM;
    context->screenif.width = 540;
    context->screenif.height = 960;
    context->screenif.bits_per_pixel = 32;

//    context->encif = kzalloc(sizeof(*(context->encif)), GFP_KERNEL);
//    if(!context->encif){
//        kfree(context->screenif);
//	return -ENOMEM;
//    }
    context->encif.frame_rate = 2000000;
    context->encif.profile = 66;
    context->encif.enc_status = 0;
    context->encif.set_i_frame = 0;
    context->encif.need_read_frame = 0;

//    context->encdt = kzalloc(sizeof(*(context->encdt)), GFP_KERNEL);
//    if(!context->encdt){
//        kfree(context->screenif);
//	kfree(context->encif);
//	return -ENOMEM;
//    }
    context->encdt.size = 0;
    context->encdt.data = (unsigned char* )vmalloc(sizeof(unsigned char) * context->screenif.width
		    * context->screenif.height * context->screenif.bits_per_pixel / 8);
//    context->encdt.data = (unsigned char* )vmalloc(sizeof(unsigned char) * 50);
    if(!context->encdt.data)
	return -ENOMEM;
    return 0;
}
static void free_context(struct enc_fb_context* context){
    if(!context)
	return;
//    kfree(context->screenif);
//    kfree(context->encif);
    vfree(context->encdt.data);
//    kfree(context->encdt);
}
static int enc_fb_open(struct inode* inode, struct file* file){
    int enc_idx = iminor(inode);
    struct hlist_node* tmp;
    struct enc_fb_device* device;

    mutex_lock(&enc_devices_mtx);
    hlist_for_each_entry_safe(device, tmp, &enc_devices, hlist){
        if(MINOR(device->cdev.dev) == enc_idx)
	    break;
	device = NULL;
    }
    mutex_unlock(&enc_devices_mtx);
    if(!device){
        return -ENODEV;
    }
    file->private_data = &device->enc_context;
    return 0;
}
static ssize_t enc_fb_write(struct file* file, const char __user* buf, size_t size, loff_t *ppos){
    return size;
}
static ssize_t enc_fb_read(struct file* file, char __user* buf, size_t size, loff_t *ppos){
    return size;
}
static long enc_fb_unlocked_ioctl(struct file* file, unsigned int cmd, unsigned long arg){
//    int err = 0,
    int	retval = 0;
    struct enc_fb_context* context = file->private_data;
    void __user *argp = (void __user *)arg;
    //判断命令幻数是否匹配
    if(_IOC_TYPE(cmd) != ENC_FB_IOC_MAGIC)
	return -ENOTTY;
    //判断命令序列是否合法
    if(_IOC_NR(cmd) > ENC_FB_IOC_MAXNR)
	return -ENOTTY;
    //判断空间是否可访问
    /* VERIFY_WRITE 是 VERIFY_READ 超集 */
/*
    if (_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        err = !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
    if(err)
	return -EFAULT;
*/
    switch(cmd) {
        case ENC_FB_IOGET_SCREEN_INFO:
	    mutex_lock(&screen_info_mtx);
	    retval = raw_copy_to_user(argp, &context->screenif, sizeof(context->screenif)) ? -EFAULT : 0;
	    mutex_unlock(&screen_info_mtx);
	    break;
        case ENC_FB_IOSET_SCREEN_INFO:
	    mutex_lock(&screen_info_mtx);
	    retval = raw_copy_from_user( &context->screenif, argp,  sizeof(context->screenif)) ? -EFAULT : 0;
	    if(!retval){
	        mutex_lock(&enc_data_mtx);
		vfree(context->encdt.data);
                context->encdt.data = (unsigned char* )vmalloc(sizeof(unsigned char) * context->screenif.width
		                 * context->screenif.height * context->screenif.bits_per_pixel / 8);
                if(!context->encdt.data)
	            retval = -ENOMEM;
		mutex_unlock(&enc_data_mtx);
	    }

	    mutex_unlock(&screen_info_mtx);
	    break;
        case ENC_FB_IOGET_ENC_INFO:
	    mutex_lock(&enc_info_mtx);
	    retval = raw_copy_to_user(argp, &context->encif, sizeof(context->encif)) ? -EFAULT : 0;
	    mutex_unlock(&enc_info_mtx);
	    break;
        case ENC_FB_IOSET_ENC_INFO:
	    mutex_lock(&enc_info_mtx);
	    retval = raw_copy_from_user( &context->encif, argp,  sizeof(context->encif)) ? -EFAULT : 0;
	    mutex_unlock(&enc_info_mtx);
	    break;
	case ENC_FB_IOGET_DATA:
	    mutex_lock(&enc_data_mtx);
	    retval = raw_copy_to_user(argp, context->encdt.data, context->encdt.size) ? -EFAULT : 0;
	    if(retval){
	        mutex_unlock(&enc_data_mtx);
		return retval;
	    }
	    retval = context->encdt.size;
	    mutex_unlock(&enc_data_mtx);
	    break;
	case ENC_FB_IOSET_DATA:
	    mutex_lock(&enc_data_mtx);
	    retval = raw_copy_from_user(context->encdt.data, argp, context->encdt.size) ? -EFAULT : 0;
	    mutex_unlock(&enc_data_mtx);
	    break;
	case ENC_FB_IOSET_SIZE:
	    mutex_lock(&enc_data_mtx);
	    context->encdt.size = arg;
	    mutex_unlock(&enc_data_mtx);
	    break;
	case ENC_FB_IOSET_VSYNC:
	    mutex_lock(&enc_info_mtx);
	    context->encif.need_read_frame = arg;
	    mutex_unlock(&enc_info_mtx);
	    break;
	case ENC_FB_IOGET_VSYNC:
	    mutex_lock(&enc_info_mtx);
	    retval = context->encif.need_read_frame;
	    mutex_unlock(&enc_info_mtx);
	    break;
	default:
	    retval = -EINVAL;
	    break;
    }
    return retval;
}
static struct file_operations enc_fb_fops = {
    .owner = THIS_MODULE,
    .open = enc_fb_open,
    .write = enc_fb_write,
    .read = enc_fb_read,
    .unlocked_ioctl = enc_fb_unlocked_ioctl,
};
static void enc_device_release(struct device *dev){

}
static int __init init_enc_device(int idx){
    int ret;
    char* name;
    dev_t devno;
    char numstr[6+21] = DEVICE_NAME;
    struct enc_fb_device* enc_device;
    enc_device = kzalloc(sizeof(*enc_device), GFP_KERNEL);
    if(!enc_device)
	return -ENOMEM;
    cdev_init(&enc_device->cdev, &enc_fb_fops);
    enc_device->cdev.owner = THIS_MODULE;

    devno = MKDEV(ENC_FB_MAJOR, idx);
    ret = cdev_add(&enc_device->cdev, devno, 1);
    if(ret){
        kfree(enc_device);
	return ret;
    }
    if(enc_devices_param > 1)
	ret = snprintf(numstr, sizeof(numstr), "%s%d", DEVICE_NAME, idx);
    if(ret < 0 || (size_t)ret >= sizeof(numstr)){
        cdev_del(&enc_device->cdev);
	kfree(enc_device);
	return -EIO;
    }

    name = kzalloc(strlen(numstr) + 1, GFP_KERNEL);
    if(!name){
        cdev_del(&enc_device->cdev);
	kfree(enc_device);
	return -ENOMEM;
    }
    strcpy(name, numstr);
    enc_device->name = name;

    ret = init_context(&enc_device->enc_context);
    if(ret){
        cdev_del(&enc_device->cdev);
	kfree(enc_device);
	kfree(name);
	return ret;   
    }


    enc_device->class_dev.devt = enc_device->cdev.dev;
    enc_device->class_dev.class = enc_fb_class;
    enc_device->class_dev.release = enc_device_release;
    dev_set_name(&enc_device->class_dev, "%s", name);
    ret = device_register(&enc_device->class_dev);
    if(ret){
        cdev_del(&enc_device->cdev);
	free_context(&enc_device->enc_context);
	kfree(enc_device);
	kfree(name);
	return ret;
    }
    mutex_lock(&enc_devices_mtx);
    hlist_add_head(&enc_device->hlist, &enc_devices);
    mutex_unlock(&enc_devices_mtx);
    
    return 0;
}
static int __init enc_fb_init(void){
    int i, ret;
    struct enc_fb_device* device;
    struct hlist_node* tmp;

    if(enc_devices_param <=0 || enc_devices_param > ENC_FB_MINOR)
         return -EINVAL;   

    ret = register_chrdev_region(MKDEV(ENC_FB_MAJOR, 0), ENC_FB_MINOR, DEVICE_NAME);
    if(ret){
        printk("can not create encfb dev\n");
	return ret;
    }
    enc_fb_class = class_create(THIS_MODULE, DEVICE_NAME);
    if(IS_ERR(enc_fb_class)){
        printk("can not create encfb class\n");
	unregister_chrdev_region(MKDEV(ENC_FB_MAJOR, 0), ENC_FB_MINOR);
	return -1;
    }
    for(i = 1; i <= enc_devices_param; i++){
        ret = init_enc_device(i);
	if(ret){
            mutex_lock(&enc_devices_mtx);
	    hlist_for_each_entry_safe(device, tmp, &enc_devices, hlist){
	        cdev_del(&device->cdev);
		device_unregister(&device->class_dev);
		free_context(&device->enc_context);
		kfree(device->name);
		hlist_del(&device->hlist);
		kfree(device);
	    }
	    mutex_unlock(&enc_devices_mtx);
	    class_destroy(enc_fb_class);
	    unregister_chrdev_region(MKDEV(ENC_FB_MAJOR, 0), ENC_FB_MINOR);
	    return -1;
	}
    }
    return ret;
}
static void __exit enc_fb_exit(void){
    struct enc_fb_device* device;
    struct hlist_node* tmp;
    
    mutex_lock(&enc_devices_mtx);
    hlist_for_each_entry_safe(device, tmp, &enc_devices, hlist){
        cdev_del(&device->cdev);
        device_unregister(&device->class_dev);
	free_context(&device->enc_context);
        kfree(device->name);
        hlist_del(&device->hlist);
        kfree(device);
    }
    mutex_unlock(&enc_devices_mtx);
    class_destroy(enc_fb_class);
    unregister_chrdev_region(MKDEV(ENC_FB_MAJOR, 0), ENC_FB_MINOR);
    
}
MODULE_LICENSE("GPL");
module_init(enc_fb_init);
module_exit(enc_fb_exit);
