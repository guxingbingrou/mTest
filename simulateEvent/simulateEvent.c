#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

static char* DEV_KEYBOARD = "/dev/input/event1";
static char* DEV_MOUSE = "/dev/input/event2";

int simulate_key(int fd, int val){
    
    //key press event	
    struct input_event event;
    gettimeofday(&event.time, 0);

    event.type = EV_KEY;
    event.value = 1;
    event.code = val;
    if(write(fd, &event, sizeof(event)) == -1){
        perror("write key");
	return 0;
    }

    //tell system
    event.type = EV_SYN;
    event.value = 0;
    event.code = SYN_REPORT;
    if(write(fd, &event, sizeof(event)) == -1){
        perror("write key");
	return 0;
    }

    //key release event
    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, 0);
    
    event.type = EV_KEY;
    event.value = 0;
    event.code = val;
    if(write(fd, &event, sizeof(event)) == -1){
        perror("write key");
	return 0;
    }

    //tell system
    event.type = EV_SYN;
    event.value = 0;
    event.code = SYN_REPORT;
    if(write(fd, &event, sizeof(event)) == -1){
        perror("write key");
	return 0;
    }

    return 1;
}

int simulate_mouse(int fd, int abs_x, int abs_y){
    struct input_event event;
    gettimeofday(&event.time, 0);
/*
    //abs x pos
    event.type = EV_ABS;
    event.value = abs_x;
    event.code = ABS_X;
    if(write(fd, &event, sizeof(event)) == -1){
        perror("write mouse");
	return 0;
    }


    //abs y pos
    event.type = EV_ABS;
    event.value = abs_y;
    event.code = ABS_Y;
    if(write(fd, &event, sizeof(event)) == -1){
        perror("write mouse");
	return 0;
    }
*/


    //mouse click
    event.type = EV_KEY;
    event.value = 1;
    event.code = BTN_LEFT;
    if(write(fd, &event, sizeof(event)) == -1){
        perror("write key");
	return 0;
    }
    //tell system
    event.type = EV_SYN;
    event.value = 0;
    event.code = SYN_REPORT;
    if(write(fd, &event, sizeof(event)) == -1){
        perror("write key");
	return 0;
    }

    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, 0);
    //mouse release
    event.type = EV_KEY;
    event.value = 0;
    event.code = BTN_LEFT;
    if(write(fd, &event, sizeof(event)) == -1){
        perror("write key");
	return 0;
    }
    //tell system
    event.type = EV_SYN;
    event.value = 0;
    event.code = SYN_REPORT;
    if(write(fd, &event, sizeof(event)) == -1){
        perror("write key");
	return 0;
    }
    return 1;
}

int main(int argc, char** argv){
    int fd_keyboard = -1;
    int fd_mouse = -1;
    int i;
    struct input_event ev;
    int px = 0, py = 0;
    fd_keyboard = open(DEV_KEYBOARD, O_RDWR);
    if(fd_keyboard == -1){
        perror("open keyboard");
	exit(0);
    }

    fd_mouse = open(DEV_MOUSE, O_RDWR);
    if(fd_mouse == -1){
        perror("open mouse");
	exit(0);
    }
/*
    for(i=100; i<600; i++){
//        if(!i%30){
//	    simulate_key(fd_keyboard, KEY_A);
//	}

	simulate_mouse(fd_mouse, i+100, i);

	sleep(1);
    }
*/
    while(1){
/*        if(read(fd_keyboard, &ev, sizeof(ev)) == sizeof(ev)){
	    if(ev.type == EV_KEY){
		printf("ev.code: %d\n", ev.code);    
		printf("KEY_Q: %d\n", KEY_Q);    
	        if(ev.code == KEY_Q && ev.value == 1){
		    simulate_mouse(fd_mouse, px, py);
		}
	    }
	}
*/
	if(read(fd_mouse, &ev, sizeof(ev))){	
/*	    if(ev.type == EV_ABS){
	        if(ev.code == ABS_X)
		    px = ev.value;
		else if(ev.code == ABS_Y)
		    py = ev.value;

		    printf("pos: x=%d, y=%d\n", px, py);

	    }
*/

	    printf("type=%d,code=%d,value=%d\n",ev.type, ev.code, ev.value);
	}
    }
    close(fd_keyboard);
    close(fd_mouse);
    return 0;

}
