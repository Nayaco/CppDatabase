#ifndef STATUS_CODES_HPP
#define STATUS_CODES_HPP
namespace SakuraDB{
    
    /* Status codes */ 
    typedef uint8_t                 STATUS_CODE;
    #define SUCCESS                 ((uint8_t)0B00100000) // 0x20
    #define KEYNOTEXIST             ((uint8_t)0B01000000) // 0x40
    #define STACKOVERFLOW           ((uint8_t)0B01100000) // 0x50
    #define UNKNOW                  ((uint8_t)0B01100001) // 0x51
    #define NULLPTR                 ((uint8_t)0B01100010) // 0x52

    /* store status*/ 
    typedef uint8_t                 STATUS;
    #define SUCCESS                 ((uint8_t)0B00100000) // 0x20
    #define KEYNOTEXIST             ((uint8_t)0B01000000) // 0x40
    #define STACKOVERFLOW           ((uint8_t)0B01100000) // 0x50
    #define UNKNOW                  ((uint8_t)0B01100001) // 0x51
    #define NULLPTR                 ((uint8_t)0B01100010) // 0x52

}

#endif