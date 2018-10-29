#include "handle.h"

NODE::NODE()
{
    _id   = 0;
    _name = 0;
    _next = NULL;
}
NODE::NODE(uint32_t id, uint32_t name, const char *rname, const char *des)
{
    _id   = id;
    _name = name;
    strncpy(_rname, rname, 54);
    _rname[54] = '\0';
    strncpy(_description, des, 126);
    _description[126] = '\0';
    _next = NULL;
}//!
NODE::~NODE(){
}

HNODE::HNODE(){
    _num  = 0;
    _sortnum = 0;
    _sort = new uint16_t[MAXLEN];
    _pond = new POND[MAXLEN];
}//!
HNODE::~HNODE(){}

long HNODE::FIND_SORT(uint16_t sort)
{
    for(long i = 0; i < this->_sortnum; i++)
        if(this->_sort[i] == sort)return i;
    return -1;
}//!
long HNODE::CREATEPOND(uint16_t sort)
{
    this->_sortnum++;
    this->_sort[_sortnum-1] = sort;
    this->_pond[_sortnum-1] = NULL;
    return this->_sortnum - 1;
}//!

NODE* HNODE::ADD(uint16_t sort, uint32_t name, const char *rname, const char *description)
{
    this->_num++;
    long _wsort = FIND_SORT(sort);
    if(_wsort == -1){
        _wsort = CREATEPOND(sort);
    }
    NODE *_node = new NODE(_num, name, rname, description);
    _node->_next = this->_pond[_wsort];
    this->_pond[_wsort] = _node;
    return _node;
}//!

void HNODE::DELETE(uint16_t sort, uint32_t name)
{
    long _wsort = FIND_SORT(sort);
    if(_wsort == -1){
        return;
    }
    for(struct NODE *i = this->_pond[_wsort], *j = NULL; i != NULL;j = i, i = i->_next){
        if(i->_name == name){
            if(j == NULL){
                this->_pond[_wsort] = i->_next;
                delete i;
                return;
            } else{
                j->_next = i->_next;
                delete i;
                return;
            }
        }
    }
    return;
}//!
void HNODE::CLEAR()
{
    for(int i = 0; i < this->_sortnum; i++){
        for(NODE *j = this->_pond[i]; j != NULL;){
            NODE *_temp = j;
            j = j->_next;
            if(_temp != NULL)delete _temp;
        }
        this->_sort[i] = 0;
        this->_pond[i] = NULL;
    }
    this->_num = 0;
    this->_sortnum = 0;
    return; 
}
uint32_t HNODE::FINDBY_RNAME(uint16_t sort,const char *rname)
{
    int len = strlen(rname);
    long _wsort = FIND_SORT(sort);
    if(_wsort == -1){
        return 0;
    }
    for(struct NODE *i = this->_pond[_wsort]; i != NULL;i = i->_next){
        if(strlen(i->_rname) == len){
            int flag = 0;
            for(int j = 0; j < len; j++){
                if((i->_rname)[j] != rname[j]){
                    flag = 1;
                    break;
                }
            }
            if(!flag)return i->_name;
        }
    }
    return 0;
}//!
uint32_t  HNODE::GETBY_RNAME(uint16_t sort, const char *rname, struct RNP *Data)
{
    int len = strlen(rname);
    long _wsort = FIND_SORT(sort);
    if(_wsort == -1){
        return 0;
    }
    for(struct NODE *i = this->_pond[_wsort]; i != NULL;i = i->_next){
        if(strlen(i->_rname) == len){
            int flag = 0;
            for(int j = 0; j < len; j++){
                if((i->_rname)[j] != rname[j]){
                    flag = 1;
                    break;
                }
            }
            if(!flag){
                Data->_name = i->_name;
                strcpy(Data->_description, i->_description);
                return i->_name;
            }
        }
    }
    return 0;
}//!
uint32_t HNODE::FINDBY_ID(uint16_t sort, uint32_t id)
{
    long _wsort = FIND_SORT(sort);
    if(_wsort == -1){
        return 0;
    }
    for(struct NODE *i = this->_pond[_wsort]; i != NULL;i = i->_next){
        if(i->_id == id){
            return i->_name;
        }
    }
    return 0;
}//!

BAKUP::BAKUP()
{
    _num = 0;
    _sortnum = 0;
    _pond = new BPOND[MAXLEN];
    _vis  = new uint8_t[MAXLEN]; 
    memset(_vis, 0, sizeof(_vis));
    _snum = new uint32_t[MAXLEN];
    _sort = new uint16_t[MAXLEN];
    for(int i = 0; i < MAXLEN; i++){
        _pond[i] = new BNODE[MAXLEN>>5];
    }
}
BAKUP::~BAKUP()
{
}
uint32_t BAKUP::_addsort(uint16_t sort)
{
    this->_sortnum++;
    this->_sort[this->_sortnum - 1] = sort;
    return this->_sortnum - 1;
}
void BAKUP::_change(uint16_t sort, HNODE *db)
{
    long _sortid = _findsid(sort);
    long _wsort = db->FIND_SORT(sort);
    this->_snum[_sortid] = 0;
    NODE *_node = db->_pond[_wsort];
    this->_vis[_sortid]  = 1; 
    while(_node != NULL){
        _putin(_node, sort);
        _node = _node->_next;
        this->_snum[_sortid] ++;
    }
    return;
}
long BAKUP::_findsid(uint16_t sort)
{
    for(int i = 0; i < this->_sortnum; i++){
        if(this->_sort[i] == sort)return i;
    }
    return -1;
}//!
void BAKUP::_putin(NODE *node, uint16_t sort)
{
    this->_num++;
    uint32_t sortid = _findsid(sort);
    if(sortid == -1){
        sortid = _addsort(sort);
    }
    this->_vis[sortid] = 1;
    this->_snum[sortid]++;
    BNODE *bnode = &((this->_pond[sortid])[this->_snum[sortid] - 1]);
    bnode->_name = node->_name;
    strncpy(bnode->_rname, node->_rname, 55);
    strncpy(bnode->_description, node->_description, 127);
    return;
}//!
void BAKUP::_savebak()
{
    char filename[7] = "bak.db";
    FILE *f = fopen(filename, "wb");
    fwrite(&(this->_num), sizeof(uint32_t), 1, f);
    fwrite(&(this->_sortnum), sizeof(uint32_t), 1, f);
    for(int i = 0; i < this->_sortnum; i++){
        fwrite(&(this->_sort[i]) , sizeof(uint16_t), 1, f);
    }
    for(int i = 0; i < this->_sortnum; i++){
        fwrite(&(this->_snum[i]) , sizeof(uint16_t), 1, f);
    }
    fclose(f);
    for(int i = 0; i < this->_sortnum; i++){
        if(this->_vis[i]){
            if(this->_snum[i] != 0)_writein(i);
            else{
                char filename[5];
                uint16_t sort = this->_sort[i];
                filename[4] = '\0';
                for(int i = 0; i < 4; i++, sort >>= 4)filename[i] = 'A' + (sort & 0x000f);
                _delete(filename);
            }
        }
        this->_vis[i] = 0;
    }
    return;
}//!

void BAKUP::_recvbak(HNODE *db)
{
    db->CLEAR();
    char filename[7] = "bak.db";
    if(!fexist(filename)){
        return;
    }
    FILE *f = fopen(filename, "rb");
    fread(&(this->_num), sizeof(this->_num), 1, f);
    fread(&(this->_sortnum), sizeof(this->_sortnum), 1, f);
    for(int i = 0; i < this->_sortnum; i++){
        fread(&(this->_sort[i]) , sizeof(uint16_t), 1, f);
    }
    for(int i = 0; i < this->_sortnum; i++){
        fread(&(this->_snum[i]) , sizeof(uint16_t), 1, f);
    }
    fclose(f);
    for(int i = 0; i < this->_sortnum; i++){
        _readout(i);
    }
    for(int i = 0; i < this->_sortnum; i++){
        for(int j = 0; j < this->_snum[i]; j++){
            db->ADD(this->_sort[i], (this->_pond[i])[j]._name, (this->_pond[i])[j]._rname, (this->_pond[i])[j]._description);
        }
    }
    return;
}
void BAKUP::_readout(uint32_t sortid)
{
    char filename[5];
    uint16_t sort = this->_sort[sortid];
    filename[4] = '\0';
    for(int i = 0; i < 4; i++, sort >>= 4)filename[i] = 'A' + (sort & 0x000f);
    if(!fexist(filename)){
        return;
    }
    FILE *f = fopen(filename, "rb");
    for(int i = 0; i < this->_snum[sortid]; i++){
        fread(&(this->_pond[sortid])[i], sizeof(BNODE), 1, f);
    }
    fclose(f);
    return;
}//!
void BAKUP::_writein(uint32_t sortid)
{
    char filename[5];
    uint16_t sort = this->_sort[sortid];
    filename[4] = '\0';
    for(int i = 0; i < 4; i++, sort >>= 4)filename[i] = 'A' + (sort & 0x000f);
    FILE *f = fopen(filename, "wb");
    for(int i = 0; i < this->_snum[sortid]; i++){
        fwrite(&(this->_pond[sortid])[i], sizeof(BNODE), 1, f);
    }
    fclose(f);
    return;
}//!
void BAKUP::_clear()
{
    char dbfilename[7] = "bak.db";
    for(int i = 0; i < this->_sortnum; i++){
        char filename[5];
        uint16_t sort = this->_sort[i];
        filename[4] = '\0';
        for(int i = 0; i < 4; i++, sort >>= 4)filename[i] = 'A' + (sort & 0x000f);
        remove(filename);
    }
    _delete(dbfilename);
}//!

void _tofilename(char *dst, uint32_t rsc)
{
    for(int i = 0; i < 8; i++){
        dst[i] = (char)((rsc & 0x0000000f) + 'A');
        rsc >>= 4;
    }
    dst[8] = '\0';
    return;
}

uint32_t _toid(char *rsc)
{
    uint32_t _temp = 0;
    for(int i = 0; i < strlen(rsc); i++){
        _temp <<= 4;
        _temp += (uint8_t)(rsc[strlen(rsc) - 1 - i]) - 'A';
    }
    return _temp;
}

uint32_t _gerator(uint32_t time)
{
    uint32_t _temp = (time << 1) + (time & 0x00000001);
    return _temp;
}

bool fexist(const char *filename)
{
    std::ifstream file(filename);
    return (bool)file;
}

uint8_t _delete(char *filename)
{
    if(remove(filename)==0){
        return 0;
    }
    return 1;
}

uint8_t _writef(char *filename, void *buff, uint32_t _size)
{
    FILE *f = fopen(filename, "ab");
    if(f == NULL)return 1;
    fwrite(buff, sizeof(uint8_t), _size, f);
    fclose(f);
    return 0;
}

uint8_t _readf(char *filename, void *buff, uint32_t &_size, FILE* f)
{
    if(f == NULL)return 2;
    if(feof(f))return 1;
    int i = 0;
    while(!feof(f) && _size--){
        fread(buff, sizeof(uint8_t), 1, f);
        i++;
    }
    _size = i;
    return feof(f);
}

uint8_t _startdb(HNODE *db, BAKUP *bakup)
{
    bakup->_recvbak(db);
}
uint8_t _closedb(BAKUP *bakup)
{
    bakup->_savebak();
}

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

uint8_t infohandle(HNODE *db, BAKUP *bakup, struct RNP *data)
{
    struct RNP;
    switch(data->_cmd){
        case 1:{
            NODE* _node = db->ADD(data->_sort, data->_name, data->_rname, data->_description);
            bakup->_putin(_node, data->_sort);        
            break;
        }
        case 2:{
            db->GETBY_RNAME(data->_sort, data->_rname, data);
            break;
        }
        case 3:{
            uint32_t _name = db->FINDBY_RNAME(data->_sort, data->_rname);
            db->DELETE(data->_sort, _name);
            bakup->_change(data->_sort, db);
            break;
        }
        case 4:{
            uint32_t _name = db->FINDBY_RNAME(data->_sort, data->_rname);
            db->DELETE(data->_sort, _name);
            data->_name = _name;
            NODE* _node = db->ADD(data->_sort, data->_name, data->_rname, data->_description);
            bakup->_putin(_node, data->_sort);
            break;
        }
        default:{
            return 1;
        }
    }
    return 0;
}
