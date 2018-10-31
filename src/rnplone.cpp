#include "rnplone.h"

void _rnpparser(struct RNP *dst, struct data_in *data,uint8_t *buff)
{
    dst->_cmd = data->_cmd;
    dst->_sort = (data->_buff[0] << 8) + (data->_buff[1]);
    dst->_name = 0;
    for(int i = 2; i < 6; i++){
        dst->_name <<= 8;
        dst->_name ^= data->_buff[i]; 
    }
    memcpy(dst->_rname, data->_buff + 6, 55 * sizeof(uint8_t));
    memcpy(dst->_description, data->_buff + 61, 127 * sizeof(uint8_t));
    memcpy(buff, data->_buff + 188, (data->_size - 188) * sizeof(uint8_t));
    return;
}

void _tofilename(char *dst, uint32_t rsc)
{
    for(int i = 0; i < 8; i++){
        dst[i] = (char)((rsc & 0x0000000f) + 'A');
        rsc >>= 4;
    }
    dst[8] = '\0';
    return;
}