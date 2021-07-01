#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXLINE 40960

int main(int argc, char** argv){
    int sockfd, n;
    char recvline[MAXLINE], sendline[MAXLINE];
    struct sockaddr_in servaddr;

    if(argc !=2){
        printf("useage: ./client <ipaddress>\n");
	exit(0);
    }

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
	exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(6666);
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
        printf("inet_pton error for %s\n", argv[1]);
	exit(0);
    }

    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
	exit(0);
    }
    printf("send msg to server: \n");
    memset(sendline, 0, MAXLINE);
    strcpy(sendline, "hello world!");
    for(int count=0; count < 100; count++){
    	if(send(sockfd, sendline, strlen(sendline), 0) < 0){
    	    printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
    	    exit(0);
    	}
	sleep(1);
    }
    printf("send ok\n");
    close(sockfd);
    exit(0);
}
