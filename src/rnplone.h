#ifndef RNP_H
#define RNP_H
#include "dstructure.h"
#include <stdint.h>
#include <cstdlib>
#include <cstring>

struct RNP{
    uint8_t                   _cmd;
    uint16_t                  _sort;
    uint32_t                  _name;
    char                      _rname[55];
    char                      _description[127];       
};

void _rnpparser(struct RNP *dst, struct data_in *data, uint8_t *buff);

void _tofilename(char *dst, uint32_t rsc);
#endif