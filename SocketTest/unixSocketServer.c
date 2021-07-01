#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>

#define UNIX_EVENT "/home/zou/eventTemp"
#define MAXLINE 4096

int main(int argc, char** argv){
    int listenfd, connfd;
    struct sockaddr_un servaddr, clt_addr;
    char buff[MAXLINE];
    int n;

    if((listenfd = socket(PF_UNIX, SOCK_STREAM, 0)) == -1){
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
	exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sun_family = AF_UNIX;
    strncpy(servaddr.sun_path, UNIX_EVENT, sizeof(servaddr.sun_path)-1);
    unlink(UNIX_EVENT);


    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
	exit(0);
    }

    if(listen(listenfd, 10) == -1){
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
	exit(0);
    }

    printf("waiting for client's request ======================\n");
    while(1){
        if((connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1){
	    printf("accept socket error: %s(errno: %d)\n",strerror(errno),errno);
	    continue;
	}
	while(1){
	    n = recv(connfd, buff, MAXLINE, 0);
	    buff[n] = '\0';
	    printf("recv msg from client: %s\n",buff);
	}
	close(connfd);
    }

    close(listenfd);
}
