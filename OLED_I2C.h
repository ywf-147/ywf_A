#ifndef __OLED_I2C_H
#define	__OLED_I2C_H

#include "stm32f10x.h"
#include "hz.h"

#define OLED_ADDRESS	0x78 //通过调整0R电阻,屏可以0x78和0x7A两个地址 -- 默认0x78

extern const char *ChDay[] ;
extern const char *ChMonth[];
extern unsigned int LunarCalendarDay;


void OLED_WriteByte(u8 Addr, u8 Data);
void WriteCmd(u8 I2C_Command);
void WriteDat(u8 I2C_Data);
void OLED_Init(void);
void OLED_SetPos(u8 x, u8 y);
void OLED_Fill(u8 fill_Data);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(u8 x, u8 y, u8 ch[], u8 TextSize);
void OLED_ShowCN(u8 x, u8 y, u8 N);
void OLED_DrawBMP(u8 x0, u8 y0, u8 x1, u8 y1, u8 *BMP);
void Show_Time(u8 TmieMdeo, u8 N);

u8 getIndex_From_GBST_16(signed char hz[]);
void OLED_ShowString16(u8 x, u8 y, u8 str[]);
int LunarCalendar(int year, int month, int day);


#endif
