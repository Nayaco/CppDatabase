#ifndef DB_H
#define DB_H
#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
/*
 * Status codes 
 */
typedef uint8_t               STATUS;
#define SUCCESS               ((uint8_t)0)
#define FILENOTEXIST          ((uint8_t)1)
#define STACKOVERFLOW         ((uint8_t)2)
#define NULLPTR               ((uint8_t)3)
#define UNKNOW                ((uint8_t)4)
typedef RESULT_NODE_SEARCH    RNS;
typedef RESULT_HNODE_SEARCH   RMS;
struct RESULT_NODE_SEARCH{
    uint16_t _res;
    STATUS _status;
};
struct RESULT_HNODE_SEARCH{
    uint16_t _res;
    STATUS _status;
};

/*
 * Data 
*/ 
typedef Data LL;
struct Data{
    uint64_t                  _data[255];
    bool operator ==(const LL &B); 
};

/*
 * pond of the data base
*/
typedef struct NODE           *POND; 
typedef struct HNODE          *H_Node;
typedef struct MNODE          *M_Node;

struct NODE{
    LL                        _value;
    uint16_t                  _id;
    struct NODE               *_next;
    NODE(int id, LL val);
    ~NODE();
};
struct HNODE{
    uint16_t                  _num;
    POND                      _pond; 
    POND                      _tail;
    POND                      _pretail;
    HNODE();
    ~HNODE();
    void CREATEPOND();
    void ADD(const LL val);
    LL FIND_ID(const uint16_t id);
    uint16_t FIND_VAL(const LL val);
};

struct MNODE{
    uint16_t                  _num;
    uint16_t                  *_rela;
    MNODE();
    ~MNODE();
    STATUS ADDNODE();
    RNS SEARCH();
};

LL hash(){

}

#ifdef __cplusplus
}
#endif
#endif