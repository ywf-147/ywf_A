#ifndef _DRV_DATETIME_YDM_H_
#define _DRV_DATETIME_YDM_H_

#include "stm32f10x.h"


typedef struct
{
	u8 year;	  //年
	u8 month;   //月
	u8 date;	  //日
	u8 day;	    //星期
	u8 hour;	  //时
	u8 min;	    //分
	u8 sec;	    //秒
	u8 isUsefull; //信息是否有效
} _DateTime;


extern _DateTime GpsDateTime; //GPS模块获取的日期时间信息
extern _DateTime SysDateTime; //系统日期时间信息

extern unsigned char year_moon,month_moon,day_moon;


//将指定日期往后加一天
void IncreaseDate(_DateTime *timp);
//时间走一秒
void Increase1sec(_DateTime *timp);
//自动计算日期时间结构体内的星期数
u8 AutoWeekStruct(_DateTime *timp);
void Conversion(_Bool c,unsigned char year,unsigned char month,unsigned char day);





#endif
