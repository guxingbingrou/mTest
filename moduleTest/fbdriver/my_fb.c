#include <linux/init.h>
#include <linux/module.h>
#include <linux/fb.h>
#include <linux/dma-mapping.h>
#include <linux/vmalloc.h>
#define X 540
#define Y 320

//u32 p_addr;
static u8 *v_addr;

struct fb_ops fops = {

};

struct fb_info *fbi;
static int __init test_init(void){
//    v_addr = dma_alloc_coherent(NULL, X*Y*4, &p_addr, GFP_KERNEL);
    v_addr = (u8 *)vmalloc(sizeof(u8) * X*Y*4);
    fbi = framebuffer_alloc(100, NULL);

    fbi->var.xres = X;
    fbi->var.yres = Y;
    fbi->var.xres_virtual = X;
    fbi->var.yres_virtual = Y;
    fbi->var.bits_per_pixel = 32;
    fbi->var.red.offset = 16;
    fbi->var.red.length = 8;
    fbi->var.green.offset = 8;
    fbi->var.green.length = 8;
    fbi->var.blue.offset = 0;
    fbi->var.blue.length = 8;

    strcpy(fbi->fix.id, "myfb");
    fbi->fix.smem_start = NULL;
    fbi->fix.smem_len = X*Y*4;
    fbi->fix.type = FB_TYPE_PACKED_PIXELS;
    fbi->fix.visual = FB_VISUAL_TRUECOLOR;
    fbi->fix.line_length = X*4;

    fbi->fbops = &fops;
    fbi->screen_base = v_addr;
    fbi->screen_size = X*Y*4;

    return register_framebuffer(fbi);
}

static void __exit test_exit(void){
    unregister_framebuffer(fbi);
    framebuffer_release(fbi);
    vfree(v_addr);
//    dma_free_coherent(NULL, X*Y*4, v_addr, p_addr);
}
module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
