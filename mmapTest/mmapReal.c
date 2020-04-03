#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PAGE_SIZE 4096

int main(int argc, char** argv){
    int fd;
    int i;
    unsigned char *p_map;

    if(argc != 2){
        printf("usage: run [option]!\n");
	exit(0);
    }

    if((fd = open("test.dat", O_RDWR | O_APPEND | O_CREAT)) < 0){
        printf("open file error\n");
	exit(0);
    }

    p_map = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(p_map == MAP_FAILED){
        printf("mmap failed!\n");
	goto end;
    }

    for(i=0;i<PAGE_SIZE;i++){
        if(p_map[i] == 0){
	    break;
	}
    }

    printf("original file length is: %d\n", i);
    printf("original file data is: %s\n", p_map);

    char* line = argv[1];
    printf("additional line length is: %ld\n", strlen(line));

    memcpy(p_map, line, strlen(line));
    msync(p_map, strlen(line), MS_SYNC);

    close(fd);

end:
    munmap(p_map, PAGE_SIZE);
    return 0;
}
