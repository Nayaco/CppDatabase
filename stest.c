#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>

#define MAXLINE 4096
const char hello[10] = "hello";

int main(int argc, char** argv)
{
    int    listenfd,       connfd,   sockfd;
    struct sockaddr_in     servaddr, cliaddr;
    char   buff[4096];
    int    n, len = sizeof(cliaddr);
    
    if( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }
    if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family =      AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port =        htons(6666);

    if( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

    if( listen(listenfd, 10) == -1){
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

    printf("======waiting for client's request======\n");
    while(1){
        if( (connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len)) == -1){
            uint32_t address = cliaddr.sin_addr.s_addr;
            printf("%d:%d:%d:%d send", (address & 0xff000000)>>24, (address & 0x00ff0000)>>16, (address & 0x0000ff00)>>8, (address & 0x000000ff));
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        }
        sleep(5);
        cliaddr.sin_family = AF_INET;
        cliaddr.sin_port = htons(2333);
        if( connect(sockfd, (struct sockaddr*)&cliaddr, sizeof(cliaddr)) == -1){
            printf("connect error: %s(error: %d)\n", strerror(errno), errno);
            exit(0);
        }
        uint32_t address = cliaddr.sin_addr.s_addr;
        printf("%d:%d:%d:%d send | ", (address & 0x000000ff), (address & 0x0000ff00)>>8, (address & 0x00ff0000)>>16, (address & 0xff000000)>>24);
        n = recv(connfd, buff, MAXLINE, 0);
        buff[n] = '\0';
        printf("recv msg from client: %s\n", buff);
        if( send(sockfd, hello, strlen(hello), 0) == -1)
        {
            printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
            exit(0);
        }
        close(connfd);
    }
    close(sockfd);
    close(listenfd);
    exit(1);
}