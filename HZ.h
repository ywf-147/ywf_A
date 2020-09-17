#ifndef __HZ_H_YWF
#define __HZ_H_YWF


#define HZMAX 115

// ------------------  汉字字模的数据结构定义 ------------------------ //
typedef struct                  // 汉字字模数据结构
{
    signed char Index[2];               // 汉字内码索引
    char Msk[32];                       // 点阵码数据
} typFNT_GB16;

extern typFNT_GB16 const GBST_16[HZMAX];
extern const unsigned char Y8X16[];



#endif
