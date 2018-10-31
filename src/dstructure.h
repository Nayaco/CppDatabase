#ifndef DSTRUCTURE_H
#define DSTRUCTURE_H
#include <cstdint>
#define MAXLEN 4096
struct data_in{
    uint16_t  _cmd;
    uint16_t  _size;
    uint8_t   _buff[MAXLEN-4];
};
#endif