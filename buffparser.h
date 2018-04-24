#ifndef BUFFPARSER_H
#define BUFFPARSER_H
#include "dstructure.h"
void buffparse(uint8_t *buff, struct data_in *data, long _size);
void buffgenerator(uint8_t *buff, struct data_in *data);
#endif