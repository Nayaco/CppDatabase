/*
* SNAKEDB (ALPHA 0.05)SERVER
*  ______      ____    __      _ _       _    _    ______
* /  ___\_\   | \ \\  | ||    / \ \\    | || / // |   __ ||
* | ||        | |\ \\ | ||   / //\ \\   | ||/ //  | ||
* \ \\_____   | ||\ \\| ||  / /===\ \\  | |  //   | |=====
*     ---\\\  | || \ \| || / /_____\ \\ | || \\   |   ___||
*  ______|| | | ||  \ | || | ||    | || | ||\ \\  | ||___
*  \____ ///  |_||   \|_|| |_||    | || |_|| \_\\ |_____ ||
* command structure(4MB):
*  ------------------------------------------------------
* | 16b | 16b  |              8b * 4092                  |
* | cmd | size |                data                     |
* |     |      |16b |64b|8b * 55| 8b * 127   | 8b * 3900 |
* |     |      |sort|name| rname |description|  data     |
*  ------------------------------------------------------ 
*/

#include <pthread.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <queue>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "dstructure.h"
#include "buffparser.h"
#include "handle.h"

#define MAXLEN 4096
#define ran(x) (rand()%x)

typedef uint8_t  PSTATUS;
#define NORMAL   0
#define ABNORMAL 1

typedef void *(*s_func)(void *) ;

struct s_address{
    uint8_t s_addr[4];
};

uint8_t operator ==(const struct data_in &A, struct data_in &B){
    return A._cmd == B._cmd;
}

#ifdef __cplusplus
extern "C" {
#endif
struct sock_args{
    long     _inport;
    long     _cliport;
    long     _numcon;
    long     _vis;
    s_func     _func;
    PSTATUS  _status;
    HNODE   *DB;
    BAKUP   *BAK;
    pthread_mutex_t            _mlock;
    std::queue<long>           _confd;
    std::queue<uint32_t>       _addr;
    std::queue<struct data_in> _data;
};
#ifdef __cplusplus
}
#endif

void _argsclr(struct sock_args *args)
{
    std::queue<long> conempty;
    std::queue<uint32_t> addrempty;
    std::queue<struct data_in> dataempty;
    args->_inport  =   0;
    args->_cliport =   0;
    args->_numcon  =   0;
    args->_vis     =   0;
    args->_status  =   NORMAL;
    args->_confd   =   conempty; 
    args->_addr    =   addrempty;
    args->_data    =   dataempty;
    pthread_mutex_init(&(args->_mlock), NULL);
    return;
}

struct s_address _toaddr(uint32_t addr){
    struct s_address _temp;
    _temp.s_addr[0] = (addr & 0xff000000)>>24;
    _temp.s_addr[1] = (addr & 0x00ff0000)>>16;
    _temp.s_addr[2] = (addr & 0x0000ff00)>>8;
    _temp.s_addr[3] = (addr & 0x000000ff);
    return _temp;
}

/*
* the accessable socket
* @param ARGS *args
* @return statuscode 0(normal)/1(abnormal)
*/
void *sock_in(void *args)
{
    printf("INFO: the access_port is running\n");
    struct sock_args    *_args = (struct sock_args *)args;
    struct sockaddr_in  servaddr, cliaddr;
    struct data_in      _tdata; 
    uint8_t             buff[MAXLEN + 4];
    long                listenfd, connfd;
    long                n;
    uint32_t            len = sizeof(cliaddr);
    
    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("ERROR: ERROR OCCUR WHEN ACC_SOCKET CREATE\n");
        pthread_exit(NULL);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      =      AF_INET;
    servaddr.sin_addr.s_addr =      htonl(INADDR_ANY);
    servaddr.sin_port        =      htons(_args->_inport); 
    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        printf("ERROR: ERROR OCCUR WHEN BIND ACC_SOCKET\n");
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        pthread_exit(NULL);
    }
    if( listen(listenfd, 10) == -1){
        printf("ERROR: ERROR OCCUR WHEN START LITEN ACC_SOCKET\n");
        pthread_exit(NULL);
    }

    while(1){
        if((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len)) == -1){
            struct s_address _temp = _toaddr(cliaddr.sin_addr.s_addr);
            printf("ERROR: ERROR OCCUR WHEN ACCESS FROM %d:%d:%d:%d\n", _temp.s_addr[3], _temp.s_addr[2], _temp.s_addr[1], _temp.s_addr[0]);
            continue;
        }
        struct s_address _temp = _toaddr(cliaddr.sin_addr.s_addr);
        printf("INFO: ACCESS FROM %d:%d:%d:%d\n", _temp.s_addr[3], _temp.s_addr[2], _temp.s_addr[1], _temp.s_addr[0]);
        n = recv(connfd, buff, MAXLEN + 4, 0);
        buff[n] = '\0';

        buffparse(buff, &_tdata, n - 4);

        pthread_mutex_lock(&(_args->_mlock));
        _args->_numcon++;
        _args->_data.push(_tdata);
        _args->_confd.push(connfd);
        _args->_addr.push(cliaddr.sin_addr.s_addr);
        pthread_mutex_unlock(&(_args->_mlock));
    }
    
    close(listenfd);
    pthread_exit(NULL);
}
/*
* the sendable socket
* @param ARGS *args
* @return statuscode 0(normal)/1(abnormal)
*/
void *sock_out(void *args)
{
    printf("INFO: the send_port is running\n");
    struct sock_args    *_args = (struct sock_args *)args;
    struct sockaddr_in  cliaddr;
    struct data_in      _tdata, _todata;
    RNP                 _rnp;
    long                _connfd;
    uint32_t            _address;
    uint8_t             _buff[MAXLEN];
    uint8_t             _tobuff[MAXLEN];  
    long                n, len = sizeof(cliaddr); 
    HNODE              *_db;
    BAKUP              *_bak;

    _db  = _args->DB;
    _bak = _args->BAK;   

    while(1){
        if(_args->_numcon == 0){
            usleep(2000);
            continue;
        }

        pthread_mutex_lock(&(_args->_mlock));

        _address = _args->_addr.front();
        _args->_addr.pop();

        _tdata = _args->_data.front();
        _args->_data.pop();
        
        _connfd = _args->_confd.front();
        _args->_confd.pop();
        
        _args->_numcon--;
        
        pthread_mutex_unlock(&(_args->_mlock));
        
        _rnpparser(&_rnp, &_tdata, _buff);
        if(_rnp._cmd == 1 || _rnp._cmd == 4){
            char filename[9];
            filename[8] = '\0';
            _tofilename(filename, _rnp._name);
            uint8_t _tmp = _writef(filename, _buff, _tdata._size - 188);
            _todata._cmd = _tmp;
            _todata._size = 0;

            infohandle(_db, _bak, &_rnp);

            buffgenerator(_tobuff, &_todata);
            if(send(_connfd, _tobuff, sizeof(_tobuff), 0) == -1){
                s_address _temp = _toaddr(_address);
                printf("ERROR: ERROR OCCUR WHEN SEND TO %d:%d:%d:%d\n",_temp.s_addr[3], _temp.s_addr[2], _temp.s_addr[1], _temp.s_addr[0]);
            }
            printf("INFO: MESSAGE HAS BEEN SENT TO CLIENT\n");
        }
        if(_rnp._cmd == 2){
            char filename[9];
            filename[8] = '\0';

            infohandle(_db, _bak, &_rnp);
            if(_rnp._name == 0)_todata._cmd = 0;
            else{
                uint32_t idname = _rnp._name;
                _todata._cmd = 1;
                _todata._buff[2] = (idname >> 24) & 0xff; 
                _todata._buff[3] = (idname >> 16) & 0xff; 
                _todata._buff[4] = (idname >> 8) & 0xff; 
                _todata._buff[5] = idname & 0xff;
                memcpy(_todata._buff + 6, _rnp._rname, 55 * sizeof(uint8_t));
                memcpy(_todata._buff + 61, _rnp._description, 127 * sizeof(uint8_t));
            }
            _todata._size = 4096;

            buffgenerator(_tobuff, &_todata);
            if(send(_connfd, _tobuff, MAXLEN, 0) == -1){
                s_address _temp = _toaddr(_address);
                printf("ERROR: ERROR OCCUR WHEN SEND TO %d:%d:%d:%d\n",_temp.s_addr[3], _temp.s_addr[2], _temp.s_addr[1], _temp.s_addr[0]);
            }
            printf("INFO: MESSAGE HAS BEEN SENT TO CLIENT\n");
        }
        if(_rnp._cmd == 3){
            char filename[9];
            filename[8] = '\0';
            uint32_t idname = _db->FINDBY_RNAME(_rnp._sort, _rnp._rname);
            _tofilename(filename, idname);
            uint16_t _tmp = _delete(filename);
            _todata._cmd = _tmp; 
            _todata._size = 0;

            infohandle(_db, _bak, &_rnp);

            buffgenerator(_tobuff, &_todata);
            if(send(_connfd, _tobuff, sizeof(_tobuff), 0) == -1){
                s_address _temp = _toaddr(_address);
                printf("ERROR: ERROR OCCUR WHEN SEND TO %d:%d:%d:%d\n",_temp.s_addr[3], _temp.s_addr[2], _temp.s_addr[1], _temp.s_addr[0]);
            }
            printf("INFO: MESSAGE HAS BEEN SENT TO CLIENT\n");    
        }

        close(_connfd);
    }

    pthread_exit(NULL);
}

void *timer(void *args){
    struct sock_args *_args = (struct sock_args *)args;
    BAKUP *_bak = _args->BAK;  
    while(1){
        sleep(10);
        pthread_mutex_lock(&(_args->_mlock));
        _bak->_savebak();
        pthread_mutex_unlock(&(_args->_mlock));
    }
    pthread_exit(NULL);
}

void S_Start(uint32_t inport, uint32_t cliport){
    HNODE DB = HNODE();
    BAKUP BAK = BAKUP();
    _startdb(&DB, &BAK);

    long             rc;
    pthread_t       _inthread, _outthread, _savethread;
    pthread_attr_t   attr;
    struct sock_args args;
    _argsclr(&args);
    args._inport  = inport;
    args._cliport = cliport;
    args.DB       = &DB;
    args.BAK      = &BAK;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    rc = pthread_create(&_inthread, &attr, sock_in, (void *)&args);
    if(rc){
        printf("ERROR: ERROR OCCUR WHEN CREATE IN_SOCKET : %d\n", (int)rc);
    }
    rc = pthread_detach(_inthread);
    if(rc){
        printf("ERROR: ERROR OCCUR WHEN JOIN IN_SOCKET : %d\n", (int)rc);
    }

    rc = pthread_create(&_outthread, &attr, sock_out, (void *)&args);
    if(rc){
        printf("ERROR: ERROR OCCUR WHEN CREATE OUT_SOCKET : %d\n", (int)rc);
    }
    rc = pthread_detach(_outthread);
    if(rc){
        printf("ERROR: RROR OCCUR WHEN JOIN OUT_SOCKET : %d\n", (int)rc);
    }

    rc = pthread_create(&_savethread, &attr, timer, (void *)&args);
    if(rc){
        printf("ERROR: ERROR OCCUR WHEN CREATE TIMER_SOCKET : %d\n", (int)rc);
    }
    rc = pthread_detach(_savethread);
    if(rc){
        printf("ERROR: RROR OCCUR WHEN JOIN TIMER_SOCKET : %d\n", (int)rc);
    }
    while(1){char ch = getchar();if(ch == 'q')break;}
    _closedb(&BAK);
    return;
}

int main(){
    printf("--------THREAD ON RUNNING--------\n");
    S_Start(2332, 2333);
    return 0;
}