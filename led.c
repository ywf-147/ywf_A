#include "main.h"
#include "led.h"

void LED_Init(void){ //LED灯的接口初始化
//	GPIO_InitTypeDef  GPIO_InitStructure; 	
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);       
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ; //选择端口号（0~15或all）                        
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //选择IO接口工作方式       
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //设置IO接口速度（2/10/50MHz）    
//	GPIO_Init(LEDPORT, &GPIO_InitStructure);	
	
	RCC->APB2ENR |=1<<4;//打开GPIOC的时钟
	GPIOC->CRH &=0XFF0FFFFF;
	GPIOC->CRH |=0X00300000;//将PC13配置为推挽输出
}
 
/*********************************************************************************************
 * 杜洋工作室 www.DoYoung.net
 * 洋桃电子 www.DoYoung.net/YT 
*********************************************************************************************/


/*
选择IO接口工作方式：
GPIO_Mode_AIN 模拟输入
GPIO_Mode_IN_FLOATING 浮空输入
GPIO_Mode_IPD 下拉输入
GPIO_Mode_IPU 上拉输入
GPIO_Mode_Out_PP 推挽输出
GPIO_Mode_Out_OD 开漏输出
GPIO_Mode_AF_PP 复用推挽输出
GPIO_Mode_AF_OD 复用开漏输出
*/
