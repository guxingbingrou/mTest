#include "enc_fb.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
//#define WIDTH 1920
//#define HEIGHT 1080
int main(void){
    int fd;
    struct enc_screen_info screenif;
    struct enc_info encif;
    struct enc_data encdt;
    fd = open("/dev/enc_fb1", O_RDWR);
    if(fd == -1){
        printf("can not open device\n");
	return -1;
    }

    if(ioctl(fd, ENC_FB_IOGET_SCREEN_INFO, &screenif)){
        printf("can not get screen info\n");
	return -1;
    }
    int screensize = screenif.width * screenif.height * screenif.bits_per_pixel / 8;
    encdt.data = (unsigned char*)malloc(sizeof(unsigned char) * screensize);
    printf("screensize=%d\n", screensize);
    printf("width=%d, height=%d, bpp=%d\n", screenif.width, screenif.height, screenif.bits_per_pixel);

/*    
    screenif.width = WIDTH;
    screenif.height = HEIGHT;
    if(ioctl(fd, ENC_FB_IOSET_SCREEN_INFO, &screenif)){
        printf("can not get screen info\n");
	return -1;
    }
    printf("set screen info\n");

    if(ioctl(fd, ENC_FB_IOGET_SCREEN_INFO, &screenif)){
        printf("can not get screen info\n");
	return -1;
    }
    printf("width=%d, height=%d, bpp=%d\n", screenif.width, screenif.height, screenif.bits_per_pixel);
*/
    
    if(ioctl(fd, ENC_FB_IOGET_ENC_INFO, &encif)){
        printf("can not get enc info\n");
	return -1;
    }
    printf("frame_rate=%d, profile=%d\n", encif.frame_rate, encif.profile);

/*
    if(ioctl(fd, ENC_FB_IOSET_ENC_INFO, &encif)){
        printf("can not get enc info\n");
	return -1;
    }
    printf("set enc info\n");

    encdt.size = screensize;
    for(int i=0;i<screensize;i++){
       encdt.data[i] = 2;
    }

    if(ioctl(fd, ENC_FB_IOSET_SIZE, screensize)){
        printf("can not set vsync\n");
	return -1;
    }

    printf("mana set data\n");   
    if(ioctl(fd, ENC_FB_IOSET_DATA, encdt.data)){
        printf("can not set encdt\n");
	return -1;
    }
    printf("set enc data \n");

    encdt.size = 0;
    for(int i=0;i<screensize;i++){
       encdt.data[i] = 0;
    }
*/

    encif.enc_status = 1;
    encif.profile = 77;
    if(ioctl(fd, ENC_FB_IOSET_ENC_INFO, &encif)){
        printf("can not get enc info\n");
        return -1;
    }
    printf("start encoder\n");


    int vsync = 0;
    int frame_count = 0;
    while(frame_count ++ < 500){
        while(!ioctl(fd, ENC_FB_IOGET_VSYNC))
	    usleep(100);
	if((encdt.size = ioctl(fd, ENC_FB_IOGET_DATA, encdt.data)) == -1){
            printf("can not get encdt\n");
            break;
        }
//        fwrite(encdt.data, encdt.size, 1, file);

        if(ioctl(fd, ENC_FB_IOSET_VSYNC, 0)){
            printf("can not set vsync\n");
            return -1;
        }

    }



/*
    vsync = ioctl(fd, ENC_FB_IOGET_VSYNC);
    if(vsync){
        if((encdt.size = ioctl(fd, ENC_FB_IOGET_DATA, encdt.data)) == -1){
            printf("can not get encdt\n");
	    return -1;
        }    
    }
    printf("try get enc data, need_read_frame=%d, size=%d,data5=%d\n", vsync, encdt.size, encdt.data[5]);


    if(ioctl(fd, ENC_FB_IOSET_VSYNC, 1)){
        printf("can not set vsync\n");
	return -1;
    }

    printf("set vsync\n");
    vsync = ioctl(fd, ENC_FB_IOGET_VSYNC);
    if(vsync){
        if((encdt.size = ioctl(fd, ENC_FB_IOGET_DATA, encdt.data)) == -1){
            printf("can not get encdt\n");
	    return -1;
        }    
    }
    printf("try get enc data, need_read_frame=%d, size=%d,data5=%d\n", vsync, encdt.size, encdt.data[screensize-1]);
 */   
    
    
    return 0;



}
