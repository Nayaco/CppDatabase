/* SNAKEDB (ALPHA 0.05)
==================================================================
 * Copyright(c) 2018 NyancoChan
 *  
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would
 *    be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 *
=================================================================
*
*  ______      ____    __      _ _       _    _    ______
* /  ___\_\   | \ \\  | ||    / \ \\    | || / // |   __ ||
* | ||        | |\ \\ | ||   / //\ \\   | ||/ //  | ||
* \ \\_____   | ||\ \\| ||  / /===\ \\  | |  //   | |=====
*     ---\\\  | || \ \| || / /_____\ \\ | || \\   |   ___||
*  ______|| | | ||  \ | || | ||    | || | ||\ \\  | ||___
*  \____ ///  |_||   \|_|| |_||    | || |_|| \_\\ |_____ ||
* command structure(4KB):
*  -------------------------------------------------------
* | 16b  | 16b  |              8b * 4092                  |
* | cmd  | size |                data                     |
* |(1234)|(no h-|16b |32b|8b * 55| 8b * 127   | 8b * 3904 |
* |(wrda)|eader)|sort|name| rname |description|  data     |
*  ------------------------------------------------------- 
*
* node structure:
*  -------------------------------------------------------
* | 16b  | 32b | 32b  |8b * 55|   8b* 127  |              |
* | sort | id  | name | rname |description |   nextnode   |
*  -------------------------------------------------------
*
* bakup structure:
*  ----------------------------
* | 32b |   32b   |            |
* | num | sortnum | sortliset  |   
*  ----------------------------
*  ------------------------------------
* | 32bX|   32b  | 8b * 55 |   8b*127  | 
* | idX |  name  |  rname  |description|
*  ------------------------------------
*/

#ifndef DB_H
#define DB_H

#include <stdint.h>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include "statusCodes.hpp"
#include "dstructure.h"


namespace SakuraDB{
#define MetaSize                1024
#define KeySize                 4096
/* RESULT_NODE_PARSER RNP */
struct RNP{
    uint8_t                     Rcmd;
    uint8_t                     Rmeta[MetaSize];
    uint8_t                     Rk   [KeySize];
    uint8_t                     *Rv;
}; 

/* ds for T-bakup */
typedef struct BNODE            **BnodeList;
typedef struct BNODE            *BnodePtr;
struct BNODE{
    uint8_t                     BnodeMeta[MAXLEN];
    uint8_t                     BnodeK[MAXLEN];
    uint8_t                     *BnodeV;
};

/* ds for T-memory */
typedef struct NODE             **NodeList;
typedef struct NODE             *NodePtr;
class NODE{
    uint32_t                    Klen;
    uint32_t                    Time;
    uint32_t                    Status;   
    uint8_t                     NodeK[MAXLEN];
    uint8_t                     *NodeV;
    NODE();
    NODE(uint32_t id, uint32_t name, const char *rname, const char *des);
    ~NODE();
};
// (build)
struct HNODE{
    uint32_t                  _num;
    uint32_t                  _sortnum;
    uint16_t                 *_sort;
    POND                     *_pond;
    HNODE();
    ~HNODE();
    long      FIND_SORT(uint16_t sort);          //not open
    //private(!!!)
    long      CREATEPOND(uint16_t sort);         //not open
    NODE*     ADD(uint16_t sort, uint32_t name, const char *rname, const char *description);
    void      DELETE(uint16_t sort, uint32_t name);
    void      CLEAR();
    uint32_t  FINDBY_RNAME(uint16_t sort, const char *rname);
    uint32_t  GETBY_RNAME(uint16_t sort, const char *rname, struct RNP *Data);
    uint32_t  FINDBY_ID(uint16_t sort, uint32_t id);
};

// stable edition(backup)
struct BAKUP{                                  
    uint32_t                  _num;
    uint32_t                  _sortnum;           // num of sort
    uint8_t                  *_vis;               // changed
    uint16_t                 *_sort;              // sort
    uint32_t                 *_snum;              // num of element in each sort
    BPOND                    *_pond;
    BAKUP();
    ~BAKUP();
    long     _findsid(uint16_t sort);
    uint32_t _addsort(uint16_t sort);
    void     _change(uint16_t sort, HNODE *db);
    void     _putin(NODE *node, uint16_t sort);
    void     _savebak();                          //open
    void     _recvbak(HNODE *db);                 //open
    void     _writein(uint32_t sortid);
    void     _readout(uint32_t sortid);
    void     _clear();                            //open
};

void         _tofilename(char *dst, uint32_t rsc);//name to filename open
uint32_t     _toid(char *rsc);                    //filename to name open
uint32_t     _gerator(uint32_t time);//open

bool          fexist(const char *filename);

uint8_t      _delete(char *filename);//0 ok /1 bad
uint8_t      _writef(char *filename, void *buff, uint32_t _size); //char or uchar 0 ok /1 bad
uint8_t      _readf(char *filename, void *buff, uint32_t _size, FILE* f);//0 ok /1 bad

uint8_t      _startdb(HNODE *db, BAKUP *bakup);   //open
uint8_t      _closedb(BAKUP *bakup);              //open

void         _rnpparser(struct RNP *dst, struct data_in *data, uint8_t *buff); //open

uint8_t       infohandle(HNODE *db, BAKUP *bakup, struct RNP *data);

}
#endif