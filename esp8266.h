#include "MAIN.h"



//#define Buf_Max 250
#define Buf_Max 1024

extern u8 WorkMode;
extern char Rx232buffer[Buf_Max]; 

//extern u8 Count;
extern u16 Count;

extern u8 esp_step;
extern u8 esp_stepSave;
extern u8 esp_delay;
extern u8 esp_cnt8;


#define ESP8266_RST_H   GPIO_SetBits(GPIOB,GPIO_Pin_0)
#define ESP8266_RST_L   GPIO_ResetBits(GPIOB,GPIO_Pin_0)

#define ESP8266_EN_H   GPIO_SetBits(GPIOB,GPIO_Pin_1)
#define ESP8266_EN_L   GPIO_ResetBits(GPIOB,GPIO_Pin_1)

/****************************************************************
AT指令集
*****************************************************************/	

#define AT_CIPCLOSE			"AT+CIPCLOSE\r\n"
#define AT_CIPSEND			"AT+CIPSEND\r\n"
#define AT_CIPMODE			"AT+CIPMODE=1\r\n"
#define AT_CIPSTART			"AT+CIPSTART=\"TCP\",\"139.199.7.215\",80\r\n"//k780网站
#define AT_CIPSTART1		"AT+CIPSTART=\"TCP\",\"39.97.30.172\",80\r\n"//高分网站
#define AT_CIPMUX0			"AT+CIPMUX=0\r\n"
#define AT_CIPMUX1			"AT+CIPMUX=1\r\n"
#define AT_CWJAP_CUR		"AT+CWJAP_CUR=\"307\",\"987654321.\"\r\n"
#define AT_RST					"AT+RST\r\n"
#define AT_CWMODE1			"AT+CWMODE=1\r\n"
#define AT_CWMODE3			"AT+CWMODE=3\r\n"
#define AT							"AT\r\n"


void esp8266_init(void);
void USART3_Init(void);
void CLR_Buf(void);
void USART3_SendByte(uint8_t byte);
void SEND_DATA(u8 *dat);
bool Hand( char *a);

void esp_waitString(u8 wait,u8 delay,u8 next);
void esp_delay5ms(u8 delay,u8 next);
void ESP8266_Connect(void);












