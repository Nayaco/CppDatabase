# 基于TCP的文件管理系统
- #### 有部分功能尚未完全实现 

```
make Dserver
make client
```
---
### 1-数据库
* 数据

这个数据库基于图状结构
``` cpp
struct NODE{
    uint32_t                  _id;
    uint32_t                  _name;//_name>0
    char                      _rname[55];
    char                      _description[127];
    struct NODE              *_next;
    NODE();
    NODE(uint32_t id, uint32_t name, const char *rname, const char *des);
    ~NODE();
};
```
对于单个节点即普通的链表节点
``` cpp
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
    uint32_t  FINDBY_RNAME(uint16_t sort,const char *rname);
    uint32_t  FINDBY_ID(uint16_t sort, uint32_t id);
};
```
_sort变量理论上应该按照文件的MD5哈希值计算得到的一个量，此处为了方便起见，在客户端中不做MD5哈希计算的实现。实际操作中单链的长度受备份单元大小限制。

数据查找我提供了两个API，一个是FINDBY_RNAME（文件名查找），一个是FINDBY_ID（ID查找）。ID查找暂时没有暴露出来，这个API设想是在服务端进行查找的内部接口。

* 备份

数据备份保证稳定可靠，所以为备份提供固定的内存单元，而不是像数据库主体使用动态空间，数据库主体主要是要对内存空间充分利用，不知道我的做法是否正确，如果有错误，欢迎指出（e-mail:nyancochan@outlook.com）

数据库每10s自动存储一次

---
### 2-网络通信
+ 应用层

主要还是在TCP上写一个应用层
```
* command structure(4KB):
*  -------------------------------------------------------
* | 16b  | 16b  |              8b * 4092                  |
* | cmd  | size |                data                     |
* |(1234)|(no h-|16b |32b|8b * 55| 8b * 127   | 8b * 3904 |
* |(wrda)|eader)|sort|name| rname |description|  data     |
*  ------------------------------------------------------- 
```
如上表命令有四条write/read/delete/append，
size是指data块的大小，
每个包固定4kB

+ Server

主要考虑到并发的情况，由于socket的队列长度有限，所以我手动维护了一个队列，基本上这还是算单线程的服务器。

因为接收与发送分离，带来的问题就是两方同时访问一个队列时出现的情况，为了维护接收与发送之间的原子性，我给这个队列加了互斥锁。


+ Client

数据分包的问题，客户端比较简单。

---
### 3-其他方面

我没做的部分：服务端向客户端发送文件包、MD5哈希

第一条和客户端向服务端发送接近，个人不想写

第二条有现成的开源库，个人比较懒没有使用

PS:文件名不能重复这条我还没有想过要解决，主要还是要MD5哈希判重

---


                            Nyancochan(ZJU)04/2018
