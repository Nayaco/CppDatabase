/*
* SNAKEDB (ALPHA 0.05)CLIENT
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
#include <cstdint>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <queue>
#include <time.h>
#include <fstream>
#include "dstructure.h"
#include "buffparser.h"
#include "rnplone.h"

#define MAXLEN 4096
#define ran(x) (rand()%x)

typedef uint8_t  PSTATUS;
#define NORMAL   0
#define ABNORMAL 1

bool fexist(const char *filename)
{
    std::ifstream file(filename);
    return (bool)file;
}

void sender(struct data_in *data, char *serverip, uint16_t port, uint8_t *rebuff)
{
    uint8_t             buff[MAXLEN];
    struct sockaddr_in  servaddr;
    long                sockfd;
    long               _size;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        printf("ERROR: ERROR OCCUR WHEN CLI_SOCKET CREATE\n");
    }

    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port =      htons(port);
    if(inet_pton(AF_INET, serverip, &servaddr.sin_addr) == -1){
        printf("ERROR: INET_PTON ERROR : %s\n",serverip);
        return;
    }
    if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1){
        printf("ERROR: CONNECT ERROR : %s\n",serverip);
        return;
    }
    
    buffgenerator(buff, data);

    if(write(sockfd, buff, sizeof(buff)) < 0){
        printf("ERROR: SEND ERROR : %s\n",serverip);
    }
    if((_size = read(sockfd, rebuff, MAXLEN)) < 0){
        printf("ERROR: GET ERROR : %s\n",serverip);
    }
    close(sockfd);
    return;
}

uint8_t _checkip(char *ip)
{
    if(strlen(ip) > 16){
        return 0;
    }
    long _temp = 0;
    for(int i = 0, k = 0, l = 0; i < 4; i++){
        _temp = -1;
        for(int j = 0; j < 3;j++, l++){    
            if(j > 0 && (ip[k + j] == '.' || ip[k + j] == '\0'))break;
            if(ip[k + j] > '9' || ip[k + j] < '0'){
                return 0;
            }else{
               if(j == 0)_temp = 0;
               _temp *= 10;
               _temp += ip[k + j] - '0';
            }
        }
        k = ++l;
        if(_temp > 255 || _temp == -1)return 0;
    }
    return 1;
}

uint32_t _getname()
{
    uint32_t t = time(0);
    t = (t << 1) + (t & 0x00000001);
    return t;
}

void _sendfile(char *filename, char *description, uint16_t sort, uint32_t name, char *ip, uint16_t port)
{
    struct data_in input, output;
    uint8_t buff[MAXLEN];
    uint8_t rebuff[MAXLEN];
    input._cmd = 1;
    input._buff[0] = sort >> 8;
    input._buff[1] = sort & 0x00ff;
    input._buff[2] = name >> 24;
    input._buff[3] = (name >> 16) & 0x00ff;
    input._buff[4] = (name >> 8) & 0x00ff;
    input._buff[5] = name & 0x00ff;
    
    std::memcpy(input._buff + 6, filename, 8 * strlen(filename));
    std::memcpy(input._buff + 61, description, 8 * strlen(description));
    if(!fexist(filename))return;
    FILE *f = fopen(filename, "rb");
    int i;
    for(i = 0; i < 3904 && !feof(f); i++)fread(buff + i, sizeof(uint8_t), 1, f);
    std::memcpy(input._buff + 188, buff, 3904 * sizeof(uint8_t));
    input._size = i + 188; 
    sender(&input, ip, port, rebuff);
    buffparse(rebuff, &output, MAXLEN);
    if(output._cmd == 1){
        printf("ERROR OCCUR WHEN WRITE TO REMOTE\n");
    }else printf("OKAY!\n");
    input._cmd = 4;
    while(!feof(f)){
        for(i = 0; i < 3904 && !feof(f); i++)fread(buff + i, sizeof(uint8_t), 1, f);
        std::memcpy(input._buff + 188, buff, 3904 * sizeof(uint8_t));
        input._size = i + 188;
        sender(&input, ip, port, rebuff);
        buffparse(rebuff, &output, MAXLEN);
        if(output._cmd == 1){
            printf("ERROR OCCUR WHEN WRITE TO REMOTE\n");
        }else printf("OKAY!\n");
    }
    fclose(f);
    return;
}

void _getfile(char *filename, uint16_t sort, char *ip, uint16_t port)
{
    struct data_in input, output;
    struct RNP _rnp;
    uint8_t buff[MAXLEN];
    uint8_t rebuff[MAXLEN];
    input._cmd = 2;
    input._size = 0;
    input._buff[0] = sort >> 8;
    input._buff[1] = sort & 0x00ff;
    std::memcpy(input._buff + 6, filename, 8 * strlen(filename));
    sender(&input, ip, port, rebuff);

    buffparse(rebuff, &output, MAXLEN); 
    _rnpparser(&_rnp, &output, buff);
    if(output._cmd == 0){
        printf("FILE NOT EXITST\n");
    }else{
        char rmtfilename[9];
        rmtfilename[8] = '\0';
        _tofilename(rmtfilename, _rnp._name);
        printf("FILE IN REMOTE : %s\n", rmtfilename);
        printf("BOOKINFO : %s\n", _rnp._description); 
    }
    return;
}

void _deletefile(char *filename, uint16_t sort, char *ip, uint16_t port)
{
    struct data_in input, output;
    uint8_t buff[MAXLEN];
    uint8_t rebuff[MAXLEN];
    input._cmd = 3;
    input._size = 0;
    input._buff[0] = sort >> 8;
    input._buff[1] = sort & 0x00ff;
    std::memcpy(input._buff + 6, filename, sizeof(filename));
    sender(&input, ip, port, rebuff);
    buffparse(rebuff, &output, MAXLEN);
    if(output._cmd == 1){
        printf("ERROR OCCUR WHEN DELETE FROM REMOTE\n");
    }else printf("OKAY!\n");

    return;
}

void _entry(){
    FILE *f = fopen("pattern", "r");
    char buff[MAXLEN];
    for(int i = 0; !feof(f); i++){
        fread(buff + i, sizeof(char), 1, f);
        buff[i + 1] = '\0'; 
    }
    printf("%s\n", buff);

    char            filename[55];
    char            description[127];
    char            serverip[31];
    uint16_t        port;
    int             _temp;
    uint8_t         _buff[MAXLEN];
    struct data_in  input, output;
    uint16_t        _cmd, sort;
    while(1){
        printf("--------PLEASE PUTIN THE SERVER IP----------\n");
        scanf("%s", serverip);
        while(!_checkip(serverip)){
            printf("--------PLEASE PUTIN THE SERVER IP----------\n");
            scanf("%s", serverip);
        }
        printf("--------PLEASE PUTIN THE SERVER PORT--------\n");
        scanf("%d", &_temp);
        while(_temp < 0 || _temp >= 25565){
            printf("--------PLEASE PUTIN THE SERVER PORT--------\n");
            scanf("%d", &_temp);
        }
        port = _temp & 0xffff;
        printf("---------PLEASE PUTIN THE COMMAND----------\n");
        printf("1-WRITE TO REMOTE\n");
        printf("2-READ FROM REMOTE\n");
        printf("3-DELETE FROM REMOTE\n");
        scanf("%d", &_temp);
        while(_temp > 3 && _temp < 1){
            printf("---------PLEASE PUTIN THE COMMAND----------\n");
            printf("1-WRITE TO REMOTE\n");
            printf("2-READ FROM REMOTE\n");
            printf("3-DELETE FROM REMOTE\n");
            scanf("%d", &_temp);
        }
        _cmd = _temp & 0xffff;
        printf("----------PLEASE PUTIN THE NAME------------\n");
        scanf("%s", filename);
        sort = (filename[0] << 8)|(filename[1]);
        switch(_cmd){
            case 1:{
                printf("-PLEASE PUTIN THE DESCRIPTION(AUTHOR AND PUBLISHJOUSE AND SOMETHINGELSE)-\n");
                scanf("%s", description);
                uint32_t name = _getname();
                _sendfile(filename, description, sort, name, serverip, port);
                break;
            }
            case 2:{
                _getfile(filename, sort, serverip, port);
                break;
            }
            case 3:{
                _deletefile(filename, sort, serverip, port);
                break;
            }
        }
        printf("GO ON?\n");
        scanf("%d", &_temp);
        if(_temp == 0)break;
    }
    return;
}
int main()
{
    _entry();
    return 0;
}