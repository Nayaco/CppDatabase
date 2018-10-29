#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     
#include <string.h> 
#include <sys/queue.h>

// for struct evkeyvalq
#include <event.h>
// for http
#include <evhttp.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/http_compat.h>
#include <event2/util.h>
#include <signal.h>
// for database
#include <pthread.h>
#include "handle.h"

#define MYHTTPD_SIGNATURE   "myhttpd v 0.0.1"
//------------------------------------------------------------------
/*
 * the object to send message
*/
#ifdef __cplusplus
extern "C" {
#endif
struct sock_args{
    HNODE                       *DB;
    BAKUP                       *BAK;
    pthread_mutex_t            _mlock;
    char                        httpd_option_listen[255];
    int                         httpd_option_port;
    int                         httpd_option_timeout;
};
#ifdef __cplusplus
}
#endif

void _argsclr(struct sock_args *args)
{
    args->httpd_option_listen = "0.0.0.0";
    args->httpd_option_port = 8080;
    args->httpd_option_timeout = 120;
    pthread_mutex_init(&(args->_mlock), NULL);
    return;
}
//---------------------------------------------------------------


void insert(struct evhttp_request *req, void *arg) {
    struct sock_args    *_args = (struct sock_args *)args;
    RNP                 _rnp;
    HNODE              *_db;
    BAKUP              *_bak;
    _db  = _args->DB;
    _bak = _args->BAK;

    char tmp[127];
    char rname[1024];
    char description[127] = "\0";

    // Ddecoded uri
    const char *uri;
    uri = evhttp_request_uri(req);
    char *decoded_uri;
    decoded_uri = evhttp_decode_uri(uri);
    // GET parser
    struct evkeyvalq params;
    // parser to 'key-value'
    evhttp_parse_query(decoded_uri, &params);
    sprintf(rname, "%s\n", evhttp_find_header(&params, "name"));
    
    sprintf(tmp, "a:%s|", evhttp_find_header(&params, "a"));
    strcat(description, tmp);
    sprintf(tmp, "p:%s|", evhttp_find_header(&params, "p"));
    strcat(description, tmp);

    free(decoded_uri);
    

    //HTTP header
    evhttp_add_header(req->output_headers, "Server", MYHTTPD_SIGNATURE);
    evhttp_add_header(req->output_headers, "Content-Type", "text/plain; charset=UTF-8");
    evhttp_add_header(req->output_headers, "Connection", "close");
    // Output
    struct evbuffer *buf;
    buf = evbuffer_new();
    evbuffer_add_printf(buf, "It's ok\n%s\n", output);
    evhttp_send_reply(req, HTTP_OK, "OK", buf);
    evbuffer_free(buf);
}

void show_help();
void signal_handler(int sig);

void *timer(void *args);
void *init_http(void *args);

void S_Start(uint32_t inport, uint32_t cliport);

int main(int argc, char *argv[]) {
    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

    char *httpd_option_listen = "0.0.0.0";
    int httpd_option_port = 8080;
    int httpd_option_daemon = 0;
    int httpd_option_timeout = 120; //in seconds
    
    int c;
    while ((c = getopt(argc, argv, "l:p:dt:h")) != -1) {
        switch (c) {
            case 'l' :
                httpd_option_listen = optarg;
                break;
            case 'p' :
                httpd_option_port = atoi(optarg);
                break;
            case 'd' :
                httpd_option_daemon = 1;
                break;
            case 't' :
                httpd_option_timeout = atoi(optarg);
                break;
            case 'h' :
            default :
                show_help();
                exit(EXIT_SUCCESS);
        }
    }

    // daemon
    if (httpd_option_daemon) {
        pid_t pid;
        pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if (pid > 0) {
            // child thread
            exit(EXIT_SUCCESS);
        }
    }

    S_Start(httpd_option_listen, httpd_option_port, httpd_option_timeout);

    return 0;
}

void show_help() {
    char *help = "http://localhost:8080\n"
        "-l <ip_addr> interface to listen on, default is 0.0.0.0\n"
        "-p <num>     port number to listen on, default is 1984\n"
        "-d           run as a deamon\n"
        "-t <second>  timeout for a http request, default is 120 seconds\n"
        "-h           print this help and exit\n"
        "\n";
    fprintf(stderr,"%s",help);
}
void signal_handler(int sig) {
    switch (sig) {
        case SIGTERM:
        case SIGHUP:
        case SIGQUIT:
        case SIGINT:
            event_loopbreak();  //stop the loop
            break;
    }
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

void *init_http(void *args){
    struct sock_args *_args = (struct sock_args*)args;
    char *httpd_option_listen = _args->httpd_option_listen;
    int httpd_option_port = args->httpd_option_port;
    int httpd_option_timeout = args->httpd_option_timeout;
    
    event_init();

    struct evhttp *httpd;
    httpd = evhttp_start(httpd_option_listen, httpd_option_port);
    evhttp_set_timeout(httpd, httpd_option_timeout);

    //generic callback
    //evhttp_set_gencb(httpd, httpd_handler, NULL);
    // or
    evhttp_set_cb(httpd, "/insert", insert, args);

    //循环处理events
    event_dispatch();
    evhttp_free(httpd);
    pthread_exit(NULL);
}

void S_Start(char *hol, int port, int timeout){
    HNODE DB = HNODE();
    BAKUP BAK = BAKUP();
    _startdb(&DB, &BAK);

    long             rc;
    pthread_t       _inthread, _savethread;
    pthread_attr_t   attr;
    struct sock_args args;
    _argsclr(&args);
    args.DB       = &DB;
    args.BAK      = &BAK;
    strcpy(args.httpd_option_listen, hol);
    args.httpd_option_port = port;
    args.httpd_option_timeout = timeout;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    rc = pthread_create(&_inthread, &attr, init_http, (void *)&args);
    if(rc){
        printf("ERROR: ERROR OCCUR WHEN CREATE HTTP : %d\n", (int)rc);
    }
    rc = pthread_detach(_inthread);
    if(rc){
        printf("ERROR: RROR OCCUR WHEN JOIN HTTP : %d\n", (int)rc);
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