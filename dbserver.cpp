#include "dbserver.h"
_dsocket::_dsocket(uint16_t)
{
    this->_port = port;
    if(this->_listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        this->_status = BIND_ERROR;
        return;
    }
    struct sockadd_in *addr = &(this->_addr);
    memset(addr, 0, sizeof(struct sockadd_in));
    (*addr).sin_family = AF_INET;
    (*addr).sin_addr.s_addr = htonl(INADDR_ANY);
    (*addr).sin_port = htons(port);

    if((bind(this->_listenfd, (struct sockaddr*)&(this->_addr), sizeof(this->addr)))== -1){
        this->_status = BIND_ERROR;
        return;
    }
    
    if(listen(this->_listenfd, 10) == -1){
        this->_status = LISTEN_ERROR; 
        return;
    }
}
_dsocket::~_dsocket()
{
    close(this->_listenfd);
}

void _dsocket::_listen(void *(*callback)(char *buff))
{ 
    long _connfd;
    while(1)
    {
        if((connfd = accept(this->_listenfd, (struct sockaddr*)NULL, NULL)) == -1){
            this->_status = CONNECT_ERROR;
            continue;
        }
        int n = recv(_connfd, this->buff, MAXLINE, 0);
        this->buff[n] = '\0';
        
        close(connfd);
    }
}

