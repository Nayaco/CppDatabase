#ifndef LOG_H
#define LOG_H

#include <fstream>
#include <stdint.h>
#include <cstdlib>
#include <queue>

#define    ran(x)          (rand()%(x))
#define    lc(x)           (x*2-1)
#define    rc(x)           (x*2)
#define    maxint          2147483647
#define    MAXLEN          255

typedef    uint8_t         LOGSTATUS;
#define    INFO            0    
#define    ALERT           1    
#define    WARNING         2    
#define    ERROR           3    

struct loginfo{
    uint32_t        _id;
    uint64_t        _time;
    LOGSTATUS       _status;
    char            _info[MAXLEN];
};

class logger{
    public:
        logger();
        ~logger();
        uint32_t    addlog();
        uint8_t     write(uint32_t id, uint64_t _time, LOGSTATUS status, char *loginfo);
    private:
        uint32_t   *_logid;
        uint32_t    _lognum; 
        uint8_t     _writetofile(struct loginfo info);
};

#endif