#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
int main(){
    int fd = open("/dev/fb1", O_RDWR);
    if(!fd){
    	printf("can not open fb1\n");
    	exit(1);
    }
    struct fb_var_screeninfo vinfo;
    if(ioctl(fd, FBIOGET_VSCREENINFO, &vinfo)){
    	printf("can not get fb1 vinfo\n");
    	exit(1);
    }
    int xres, yres;
    xres = vinfo.xres;
    yres = vinfo.yres;
    int bits_per_pixel = vinfo.bits_per_pixel;
    int screensize = xres * yres * bits_per_pixel / 8;
    printf("xres=%d, yres=%d, bits_per_pixel=%d\n", xres, yres, bits_per_pixel);
    unsigned char* fbp = (unsigned char*)mmap(NULL, screensize, PROT_READ, MAP_SHARED, fd, 0);
    if(fbp == MAP_FAILED  ){
    	printf("can not mmap fbp");
    	exit(1);
    }

    FILE* file = fopen("/home/zou/framebuffer.rgb", "wb");

    fwrite(fbp, screensize, 1, file);
//    while(1){
    
//    }
    munmap(fbp, screensize);
    close(fd);
    return 0;
}
