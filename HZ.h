#ifndef __HZ_H_YWF
#define __HZ_H_YWF


#define HZMAX 115

// ------------------  ������ģ�����ݽṹ���� ------------------------ //
typedef struct                  // ������ģ���ݽṹ
{
    signed char Index[2];               // ������������
    char Msk[32];                       // ����������
} typFNT_GB16;

extern typFNT_GB16 const GBST_16[HZMAX];
extern const unsigned char Y8X16[];



#endif
