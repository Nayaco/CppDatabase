#ifndef SERVER_H
#define SERVER_H

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef uint8_t         SSTATUS;
#define SUCCESS         ((uint8_t)0)
#define BIND_ERROR      ((uint8_t)1)
#define LISTEN_ERROR    ((uint8_t)2)
#define CONNECT_ERROR   ((uint8_t)3)

#define MAXLINE         4096
typedef dsocket        D_SOCKET;
typedef void *(*callback)();

class dsocket{
    protected:
        long                _listenfd;
        struct sockadd_in   _addr;
        char                _buff[MAXLINE];
        SSTATUS             _status;
        uint16_t            _port;
    public:
    dsocket(uint16_t port);
    ~dsocket();
    csonnect();
    void liten(callback);
};

#endif