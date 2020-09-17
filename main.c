#include "reg52.h"
#include "intrins.h"

#define DelayVal 2

sbit Key_Zheng = P2^0;
sbit Key_Fan   = P2^1;
sbit Key_Ting  = P2^2;
sbit Key_jia  = P2^3;
sbit Key_jian  = P2^4;

sbit led1 = P1^0;
sbit led2 = P1^1;
sbit led3 = P1^2;
sbit led4 = P1^3;


sbit IOP30	=	P3 ^ 0;
sfr isp_unsig = 0xc7;

void Dji_FanZhuan(void );
void Dji_ZhengZhuan(void );
void Key_Can(void);

char ZhengZhuan[4]={0x01,0x02,0x04,0x08};			//正转
char FanZhuan[4]={0x08,0x04,0x02,0x01};			  //反转


char code smgduan[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
					0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};//显示0~F的值

void Delay_Ms(unsigned int i)
{
	unsigned int y;
	while(i--)
	{
		  
		for(y=0;y<380;y++)
		{
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			_nop_();

			if(!IOP30)isp_unsig = 0x60;
			
		}
	}
	
}

char Zheng_Flag = 0 ;
char Fan_Flag = 0 ;
char Ting_Flag = 0 ;
char Dji_ShuDu = 2 ;


int main(void)
{
	
	while(1)
	{

Key_Can();
		
		if(Ting_Flag)P1=0x00;
		if(Fan_Flag)Dji_FanZhuan();
		if(Zheng_Flag)Dji_ZhengZhuan();
		P0= ~smgduan[Dji_ShuDu];
		

		Delay_Ms(1);
	}
	
}

void Key_Can(void)
{
	if(!Key_jia)
		{
			Delay_Ms(80);
			if(!Key_jia)
			{
				if(Dji_ShuDu!=9)Dji_ShuDu++;
				else Dji_ShuDu=2;
				
			}
		}
		
		if(!Key_jian)
		{
			Delay_Ms(80);
			if(!Key_jian)
			{
				
				if(Dji_ShuDu!=2)Dji_ShuDu--;
				else Dji_ShuDu=9;
				
			}
		}
		
		
		if(!Key_Zheng)
		{
			Delay_Ms(20);
			if(!Key_Zheng)
			{
				Fan_Flag = 0;
				Ting_Flag = 0;
				Zheng_Flag = 1;
				
			}
		}
		
		if(!Key_Fan)
		{
			Delay_Ms(20);
			if(!Key_Fan)
			{
				Zheng_Flag = 0;
				Ting_Flag = 0;
				Fan_Flag = 1;
				
				
			}
		}
		
		if(!Key_Ting)
		{
			Delay_Ms(20);
			if(!Key_Ting)
			{
				Fan_Flag = 0;
				Zheng_Flag = 0;
				Ting_Flag = 1;
			}
		}
}

void Dji_ZhengZhuan(void )
{
	char i;
	
	for(i=0;i<4;i++)
		{
			P1=ZhengZhuan[i];//FanZhuan
			
			Delay_Ms(Dji_ShuDu);
		}
}

void Dji_FanZhuan(void )
{
	char i;
	
	for(i=0;i<4;i++)
		{
			P1=FanZhuan[i];//FanZhuan
			
			Delay_Ms(Dji_ShuDu);
		}
}












