#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/time.h>
#include <linux/input.h>
//#include <asm/page.h>
static int xres = 0;
static int yres = 0;
static int bits_per_pixel = 0;
static unsigned char *fbp = NULL;
int main(int argc, char** argv){
    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long screensize = 0;
    struct fb_bitfield red;
    struct fb_bitfield green;
    struct fb_bitfield blue;
    unsigned long fb_offset = 0;
    fbfd = open("/dev/fb0", O_RDWR);
    if(-1 == fbfd){
        perror("open");
	exit(0);
    }

    if(ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1){
	perror("ioctl fix");
	exit(0);
    }
    
    printf("lineLength: %ld\n", finfo.line_length);
    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1){
	perror("ioctl var");    
	exit(0);
    }
    printf("R: %ld, G: %ld, B: %ld \n", vinfo.red, vinfo.green, vinfo.blue);

    printf("width: %d, height: %d, deepth: %d\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    xres = vinfo.xres;
    yres = vinfo.yres;
    bits_per_pixel = vinfo.bits_per_pixel;

    screensize = xres * yres * bits_per_pixel / 8;
    printf("screensize: %ld\n", screensize);
    
//    fb_offset = (unsigned long)(finfo.smem_start) & (~PAGE_MASK);
    
    printf("fb_offset: %ld, finfo.smem_len: %ld", fb_offset, finfo.smem_len);
//    fbp = (unsigned long int)mmap(0, finfo.smem_len + fb_offset, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    fbp = (unsigned long int)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if((int)fbp == -1){
	perror("mmap");    
	exit(0);
    }
    
    printf("%d\n", strlen(fbp));
    memset(fbp, 0, screensize);
    unsigned char color[4] = {0,255,0,0};
    double k = 0;
/*    
    while(1){
        int px = 0;
        int py = 0;
        for(;px<800;px++){
	    py = k * px;
            if(py > 600)
	        break;	    
            memcpy((void *)(fbp + py * xres * bits_per_pixel / 8 + 4*px), color, 4);
        }
	sleep(1);
	memset(fbp, 0, screensize);
	k += 0.1 * k + 0.1;
	if(k >= 20){
	    k = 0;
	    sleep(0.1);
	    memset(fbp, 0, screensize);
	    sleep(0.1);
	}
    }
*/

    int fd_mouse = -1;
    int px = 0, py = 0;
    struct input_event event;

    fd_mouse = open("/dev/input/event2", O_RDONLY);
    if(fd_mouse == -1){
        perror("mouse");
	exit(0);
    }

    memset(fbp, 0, screensize);
    int flg = 0;
    while(1){
        if( read(fd_mouse, &event, sizeof(event)) == sizeof(event)){
	    if(event.type == EV_ABS){
	        if(event.code == ABS_X)
		    px = event.value;
		else if(event.code == ABS_Y)
		    py = event.value;
		if(px > 800)
		    px = 800;
		if(py >600)
		    py = 600;
                printf("px: %d, py: %d\n", px, py);
		if(flg){
                    memcpy((void *)(fbp + py * xres * bits_per_pixel / 8 + 4*px), color, 4);
	            sleep(0.1);
		}
	    }
	    else if(event.type == EV_KEY){
	    printf("event_type: %d\n", event.type);
	    if(event.code == BTN_LEFT && event.value == 1){
	        flg = !flg;
	    }
	    }
	}
    }

    printf("draw done!\n");



    close(fbfd);
    munmap(fbp, screensize);
//    fbp = -1;
    return 0;



}
