#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define MAXLINE 4096
#define UNIX_EVENT "/home/zou/audio"
int main(int argc, char** argv){
    int sockfd, n;
    char recvline[MAXLINE], sendline[MAXLINE];
    struct sockaddr_un servaddr;


    if((sockfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0){
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
	exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX;
    strcpy(servaddr.sun_path, UNIX_EVENT);

    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
	exit(0);
    }

    printf("connect to server: \n");
    sendline[0] = 1;
    send(sockfd, sendline, 1, 0);

    memset(recvline, 0, sizeof(recvline));
    int size = recv(sockfd, recvline, MAXLINE, 0);
    printf("%d,%s\n", recvline[0], recvline);


    close(sockfd);
    exit(0);
}
