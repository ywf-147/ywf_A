#include "main.h"
#include "led.h"

void LED_Init(void){ //LED�ƵĽӿڳ�ʼ��
//	GPIO_InitTypeDef  GPIO_InitStructure; 	
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);       
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 ; //ѡ��˿ںţ�0~15��all��                        
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //ѡ��IO�ӿڹ�����ʽ       
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //����IO�ӿ��ٶȣ�2/10/50MHz��    
//	GPIO_Init(LEDPORT, &GPIO_InitStructure);	
	
	RCC->APB2ENR |=1<<4;//��GPIOC��ʱ��
	GPIOC->CRH &=0XFF0FFFFF;
	GPIOC->CRH |=0X00300000;//��PC13����Ϊ�������
}
 
/*********************************************************************************************
 * �������� www.DoYoung.net
 * ���ҵ��� www.DoYoung.net/YT 
*********************************************************************************************/


/*
ѡ��IO�ӿڹ�����ʽ��
GPIO_Mode_AIN ģ������
GPIO_Mode_IN_FLOATING ��������
GPIO_Mode_IPD ��������
GPIO_Mode_IPU ��������
GPIO_Mode_Out_PP �������
GPIO_Mode_Out_OD ��©���
GPIO_Mode_AF_PP �����������
GPIO_Mode_AF_OD ���ÿ�©���
*/
