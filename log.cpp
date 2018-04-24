#include "log.h"
logger::logger()
{
    this->_logid = new uint32_t[1024];
    for(int i = 0; i < 1024; i++)(this->_logid)[i] = (uint32_t)0;
}

logger::~logger()
{

}

uint32_t logger::addlog()
{
    if(this->_lognum == 1023)return 0;
    uint32_t _id = ran(maxint>>1);
    int _temp = 1;
    while(1){
        while((this->_logid)[_temp] != 0 && _temp < 1024){
            if(_id < (this->_logid)[_temp]){
                _temp = lc(_temp);
            }else if(_id > (this->_logid)[_temp]){
                _temp = rc(_temp);
            }else if(_id == (this->_logid)[_temp]){
                break;
            }
        }

        if((this->_logid)[_temp] == _id || _temp > 1023 || _id < (1024 - _temp)*2){
            _id = (ran(maxint-1)<<30) + (ran(maxint-1));
            continue;
        }

        if((this->_logid)[_temp] == 0){
            this->_lognum++;
            (this->_logid)[_temp] = _id;
            return _id;
        }

    }
}

uint8_t logger::write(uint32_t id, uint64_t _time, LOGSTATUS status, char *info){
    
}



uint8_t logger::_writetofile(struct loginfo info)
{
    char _logname[16];
    uint32_t _id = info._id;
    for(int i = 0; i < 16; i++){
        _logname[15 - i] = (char)(54 + (_id & 0x0000000F));
        _id >>= 2;
    }
    
}