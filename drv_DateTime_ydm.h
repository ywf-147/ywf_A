#ifndef _DRV_DATETIME_YDM_H_
#define _DRV_DATETIME_YDM_H_

#include "stm32f10x.h"


typedef struct
{
	u8 year;	  //��
	u8 month;   //��
	u8 date;	  //��
	u8 day;	    //����
	u8 hour;	  //ʱ
	u8 min;	    //��
	u8 sec;	    //��
	u8 isUsefull; //��Ϣ�Ƿ���Ч
} _DateTime;


extern _DateTime GpsDateTime; //GPSģ���ȡ������ʱ����Ϣ
extern _DateTime SysDateTime; //ϵͳ����ʱ����Ϣ

extern unsigned char year_moon,month_moon,day_moon;


//��ָ�����������һ��
void IncreaseDate(_DateTime *timp);
//ʱ����һ��
void Increase1sec(_DateTime *timp);
//�Զ���������ʱ��ṹ���ڵ�������
u8 AutoWeekStruct(_DateTime *timp);
void Conversion(_Bool c,unsigned char year,unsigned char month,unsigned char day);





#endif
