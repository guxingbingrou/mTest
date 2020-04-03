#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
int main(int argc, char** argv){
    int server_socketfd, client_socketfd;
    int server_len, client_len;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int result;
    fd_set readfds, testfds;

    server_socketfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(6666);
    server_len = sizeof(server_addr);

    bind(server_socketfd, (struct sockaddr *)&server_addr, server_len );

    listen(server_socketfd, 5);

    FD_ZERO(&readfds);
    FD_SET(server_socketfd, &readfds);

    while(1){
        char ch;
	int fd;
	int nread;
	testfds = readfds;
	printf("server waiting...\n");
        
	struct timeval tv;
	tv.tv_sec = 0;
//	tv.tv_usec = 100 * 1000; 
	tv.tv_usec = LONG_MAX;
        result = select(FD_SETSIZE, &testfds, NULL, NULL, &tv);
	if(result < 0){
	    perror("select");
	    exit(0);
	}
        if(result == 0)
	    continue;
	for(fd=0;fd<FD_SETSIZE;fd++){
	    if(FD_ISSET(fd, &testfds)){
	        if(fd == server_socketfd){
		    client_len = sizeof(client_addr);
		    client_socketfd = accept(server_socketfd, (struct sockaddr *)&client_addr, &client_len);

		    FD_SET(client_socketfd, &readfds);
		    printf("add client on fd: %d\n",client_socketfd);
		}
		else{
		    ioctl(fd, FIONREAD, &nread);
		    if(nread == 0){
		        close(fd);
			FD_CLR(fd, &readfds);
			printf("remove client on fd: %d\n", fd);
		    }
		    else{
			printf("nread: %d\n", nread);
		        read(fd, &ch, 1);
			printf("data: %c\n", ch);
			printf("serving client on fd: %d\n", fd);
		    }
		}
	    }
	}

    }
    return 0;
}
