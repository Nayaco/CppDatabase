#include "buffparser.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
void buffparse(uint8_t *buff, struct data_in *data, long _size){
    data->_cmd  = (buff[0] << 8) + (buff[1]);
    data->_size = (buff[2] << 8) + (buff[3]);
    if(data->_size > _size || data->_size == 0)data->_size = (uint32_t)_size;
    if(data->_size < 4092)data->_size --;
    std::memcpy(data->_buff, buff + 4, 4092);
}

void buffgenerator(uint8_t *buff, struct data_in *data){
    buff[0] = data->_cmd >> 8;
    buff[1] = data->_cmd & 0x00ff;
    buff[2] = data->_size >> 8;
    buff[3] = data->_size & 0x00ff;
    std::memcpy(buff + 4, data->_buff, 4092);
}