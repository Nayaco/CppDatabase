#include "DB.h"

NODE::NODE(int id, LL val)
{
    _value = val;
    _next = nullptr;
    _id = id;
}
NODE::~NODE(){}

HNODE::HNODE(){}
HNODE::~HNODE(){}
void HNODE::CREATEPOND()
{
    _num = 0;
    _pond = new NODE(0, -1);
    _tail = new NODE(0, -1);
    _pond->_next = _tail;
}

void HNODE::ADD(const LL val)
{
    NODE *Temp = new NODE(++_num, val);
    Temp->_next = _pond->_next;
    _pond->_next = Temp;  
}

LL HNODE::FIND_ID(const uint16_t id)
{
    NODE *Temp = _pond->_next;
    while(Temp->_id != 0)
    {
        if(Temp->_id == id)
        {
            return Temp->_value;
        }
        Temp = Temp->_next;
    }
    return -1;
}

uint16_t HNODE::FIND_VAL(const LL val)
{
    NODE *Temp = _pond->_next;
    while(Temp->_id != 0)
    {
        if(Temp->_value == val)
        {
            return Temp->_id;
        }
        Temp = Temp->_next;
    }
    return -1;
}

MNODE::MNODE(){
    _num = 0;
}

MNODE::~MNODE(){
}

STATUS MNODE::ADDNODE(){
    
}