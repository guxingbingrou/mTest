#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define MAXLINE 4096
#define UNIX_EVENT "/run/user/1000/anbox/instance1/input/event0"
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
	  struct CompatEvent {
	    // NOTE: A bit dirty but as we're running currently a 64 bit container
	    // struct input_event has a different size. We rebuild the struct here
	    // to reach the correct size.
	    u_int64_t sec;
	    u_int64_t usec;
	    u_int16_t type;
	    u_int16_t code;
	    u_int32_t value;
	  };
	  union RECVDATA{
	      struct CompatEvent comData;
	      char data[24];
	  };
	  printf("size:%d,%d\n",sizeof(struct CompatEvent), sizeof(union RECVDATA));
	union RECVDATA mdata;
    while(1){
/*    	fgets(sendline, MAXLINE, stdin);
    	if(send(sockfd, sendline, strlen(sendline), 0) < 0){
            printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
            exit(0);
   	 }
*/	 
	memset(recvline, 0, sizeof(recvline));
        int size = recv(sockfd, recvline, MAXLINE, 0);
	char *temp = recvline;
	int oldsize = size;
	while(size > 0){
	    strncpy(mdata.data, temp, 24);
	    for(int i=0;i<24;i++)
	        printf("data[%d]:%d\n",i,mdata.data[i]);
	    temp+=24;
	    size-=24;
	    //printf("sec:%ld,usec:%ld,type:%d,code:%d,value:%d\n",mdata.comData.sec, mdata.comData.usec, mdata.comData.type, mdata.comData.code, mdata.comData.value);
	}
	printf("recv msg from server: %s, size: %d\n", recvline, oldsize);
//	sleep(0.01);
    }
/*    fgets(sendline, MAXLINE, stdin);
    if(send(sockfd, sendline, strlen(sendline), 0) < 0){
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
	exit(0);
    }
*/
    close(sockfd);
    exit(0);
}
