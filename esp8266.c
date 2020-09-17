#include "main.h"         
#include "esp8266.h"

//===============================================

u8 test=0;
//u8 Count=0;  
u16 Count=0;  
// ���������ۼӼ���       
char Rx232buffer[Buf_Max];                                         // ��������
u8 WorkMode=1;//����ģʽ

#define SET_ESP_STA    			0x80		//����ΪSTAģʽ
#define SET_ESP_AP     			0x40		//����ΪAPģʽ
#define SET_ESP_AP_STA 			0x20		//����ΪAP+STAģʽ
#define SET_ESP_GET_TIME		0x10		//���û�ȡʱ��
#define SET_ESP_GET_WEATHER	0x08		//���û�ȡ����

#define ESP_ERROR      0x80
#define ESP_TIMEOUT    0x40

#define WAIT_OK     0x80
#define WAIT_GOTIP  0x40
#define WAIT_READY  0x20
#define WAIT_CONNECT  0x10
u8 a=0;
u8 esp_step=0,esp_stepNext,esp_stepSave;
u8 esp_cnt8,esp_delay,esp_waitflg=0;


void esp_waitString(u8 wait,u8 delay,u8 next)
{
	esp_stepSave=esp_step;//���浱ǰ���裨���ֵȴ���ʱ���߷���ERRORʱ������֪���Ǵ��ĸ�case��֧�����ģ�
	esp_stepNext=next;	
	esp_waitflg=wait;
	esp_delay=delay;
	CLR_Buf();//�������
	esp_cnt8=0;
	esp_step=255;

}
void esp_delay5ms(u8 delay,u8 next)
{
	esp_delay=delay;
	esp_stepNext=next;
	esp_step=254;
}

//===============================================



			
/**************************************************************************************
 * ��  �� : ��ʼ��ESP8266ģ���õ�������
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************/
void esp8266_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  //������GPIO
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 
  
  //���õ�IO��PB10������3��TXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  //���õ�IO��PB11������3��RXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;     //��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
     
  //���õ�IO��PB6 PB7��ESP8266��λ����RST
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;           //�������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  //��ʼ״̬��ESP8266��λ����ʹ���ø�
  ESP8266_RST_H;
	ESP8266_EN_H;
}

/**************************************************************************************
 * ��  �� : ��ʼ��USART3������USART3�ж����ȼ�
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************/
void USART3_Init(void)
{
	USART_InitTypeDef				USART_InitStructure;
	USART_ClockInitTypeDef  USART_ClockInitStructure;
	NVIC_InitTypeDef				NVIC_InitStructure;

  //�򿪴���3��ʱ��
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3 , ENABLE); 
	
  //���ô���3��Ӳ������
	USART_DeInit(USART3);                                            //������USART3�Ĵ�������Ϊȱʡֵ
	USART_InitStructure.USART_BaudRate = 115200;                      //���ô���3������Ϊ115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;      //����һ��֡�д�������λ 
	USART_InitStructure.USART_StopBits = USART_StopBits_1;           //���巢�͵�ֹͣλ��ĿΪ1
	USART_InitStructure.USART_Parity = USART_Parity_No;              //��żʧ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //Ӳ��������ʧ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                     //���ͽ���ʹ��
	USART_Init(USART3, &USART_InitStructure);    

  USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;      //ʱ�ӵ͵�ƽ�
  USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;            //����SCLK������ʱ������ļ���Ϊ�͵�ƽ
  USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;          //ʱ�ӵ�һ�����ؽ������ݲ���
  USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;  //���һλ���ݵ�ʱ�����岻��SCLK���
  USART_ClockInit(USART3, &USART_ClockInitStructure); 
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);                   //ʹ��USART3�����ж�
	USART_Cmd(USART3, ENABLE);                                       //ʹ��USART3����
	
  //���ô���3�ж����ȼ�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                  //NVIC_Group:��ռ���ȼ�2λ�������ȼ�2λ
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	               //����Ϊ����3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;          //��ռ���ȼ�Ϊ2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	             //�����ȼ�Ϊ3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                  //ʹ���ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);
	
}

/**************************************************************************************
 * ��  �� : USART3���ͺ���
 * ��  �� : �����ַ�
 * ����ֵ : ��
 **************************************************************************************/
void USART3_SendByte(uint8_t byte)
{
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC) != SET);
	USART_SendData(USART3, byte);
}

void SEND_DATA(u8 *dat)
{
	while(*dat!='\0')
	{
		USART3_SendByte(*dat);
		dat++;
		
	}
	
}

/**************************************************************************************
 * ��  �� : ���ֳɹ������
 * ��  �� : char *a���Ա��ַ���
 * ����ֵ : ��
 **************************************************************************************/
bool Hand( char *a)
{ 
  if(strstr(Rx232buffer,a)!=NULL)    //�ж�ָ��a�е��ַ����Ƿ���Rx232buffer�������ַ������Ӵ�
	   return true;
	else
		 return false;
}

/**************************************************************************************
 * ��  �� : ��ս�����������
 * ��  �� : 1Ϊ������ģʽ��//������棬2Ϊ����ģʽ��//�������
 * ����ֵ : ��
 **************************************************************************************/
void CLR_Buf(void)
{
		memset(Rx232buffer,0,Buf_Max);
		 Count = 0;                                         //���������ۼӼ������㣬���´ν��մ�ͷ��ʼ
}


/**************************************************************************************
 * ��  �� : USART3ȫ���жϷ���
 * ��  �� : ��	WorkMode
 * ����ֵ : ��
 **************************************************************************************/
void USART3_IRQHandler(void)
{  
	u8 tmp=0;
  if(USART_GetITStatus(USART3,USART_IT_RXNE)!= RESET)       //�����ж�
  {
		tmp= USART_ReceiveData(USART3);  
		if(WorkMode==1){//��������ģʽ��						
			Rx232buffer[Count]=tmp;
			Count++;                                                //�����ۼӽ�������                          
			if(Count>=Buf_Max)                                       //���������ڶ����������������ʱ����ͷ��ʼ��������
			{
				 Count = 0;
			} 		
		}
       //ͨ������USART3��������		
    USART_ClearITPendingBit(USART3, USART_IT_RXNE);         //���USART3���жϴ�����λ
  }  
}


void ESP8266_Connect(void)
{
		switch(esp_step)
		{
			case 0:	
				

				break;//esp_stepΪ0��ʾ�ر�	
			case 1:	
				
				esp_step=101;
				break;//esp_s
			case 10://   Ӳ����λ             "1234567890123456"
				//display_GB2312_string(1,1,(u8 *)"1. Ӳ����λ     ");
				ESP8266_RST_L;	
				//esp_delay5ms(100,11);//������ʱ100*5ms��Ȼ��תcase 11
				esp_delay5ms(10,11);//�ȴ�100ms��Ȼ��ת11���裨100ms��20*5ms�����ģ�
				break;
			case 11:
				ESP8266_RST_H;//��ESP8266��λһ�Σ��˲����ز�����	
				esp_waitString(WAIT_READY,10,20);//WAIT_OK   WAIT_GOTIP  WAIT_READY	
				//esp_step=20;
				break;	
			
			case 20://                        "1234567890123456"
				//display_GB2312_string(1,1,(u8 *)"2.����AT        ");
				esp_waitString(WAIT_OK,5,30);//WAIT_OK   WAIT_GOTIP  WAIT_READY	
				SEND_DATA((u8 *)AT); //��������ָ��(u8 *)"AT"
				break;
			case 30://                        "1234567890123456"
				//display_GB2312_string(1,1,(u8 *)"3.��ΪSTAģʽ   ");
				esp_waitString(WAIT_OK,5,40);//WAIT_OK   WAIT_GOTIP  WAIT_READY		
				SEND_DATA((u8 *)AT_CWMODE1); //����ESP8266����ģʽ CWMODE=3ΪAP+STATIONģʽ CWMODE=1ΪSTAģʽ
				break;
			case 40://                        "1234567890123456"
				//display_GB2312_string(1,1,(u8 *)"4.����ģ��(����)");
				esp_waitString(WAIT_OK|WAIT_READY,10,45);//WAIT_OK��WAIT_GOTIP	
				SEND_DATA((u8 *)AT_RST);  //����ģ�飨�����λ��	
				break;
			
			case 45://                        "1234567890123456"
				//display_GB2312_string(1,1,(u8 *)"x.����307WiFi   ");
				esp_waitString(WAIT_GOTIP,20,50);//WAIT_OK��WAIT_GOTIP��WAIT_READY	
				SEND_DATA((u8 *)AT_CWJAP_CUR);                     //��������ָ��(u8 *)"AT+CWJAP=\"307\",\"987654321.\""
				break;	
			
			case 50://                        "1234567890123456"
				
				esp_waitString(WAIT_OK,50,60);	//WAIT_OK   WAIT_GOTIP  WAIT_READY	
				SEND_DATA((u8 *)AT_CIPMUX0);  //����ESP8266����ģʽ  CIPMUX=1������  CIPMUX=0�����ӣ�͸����
				break;
			
			case 54://�Ͽ�����������Ϊ����K780��������׼��	·��Ϊ 54 60 70 80 90 
				esp_waitString(WAIT_OK,50,60);	//WAIT_OK   WAIT_GOTIP  WAIT_READY
				SEND_DATA((u8 *)AT_CIPCLOSE);//�Ͽ�����������
				break;
			
			case 55://�Ͽ�����������Ϊ���Ӹ߷ַ�������׼�� ·��Ϊ55 65 75 85 100
				esp_waitString(WAIT_OK,50,65);	//WAIT_OK   WAIT_GOTIP  WAIT_READY
				SEND_DATA((u8 *)AT_CIPCLOSE);//�Ͽ�����������
				break;
			
			case 60://                        "1234567890123456"
				
				esp_waitString(WAIT_OK,80,70);//WAIT_OK   WAIT_GOTIP  WAIT_READY	
			//SEND_DATA((u8 *)"AT+CIPSERVER=1,4000\r\n");    //����TCP�������ҿ��Ŷ˿�Ϊ4000����ʱ��IP��ַ����·�������䣬������Ϊ�̶�IP��
				SEND_DATA((u8 *)AT_CIPSTART);    //���ӵ�TCP������ָ���˿�
				break;
			
			case 65://   ���Ӹ߷ַ�����      
				
				esp_waitString(WAIT_OK,80,75);//WAIT_OK   WAIT_GOTIP  WAIT_READY	
			//SEND_DATA((u8 *)"AT+CIPSERVER=1,4000\r\n");    //����TCP�������ҿ��Ŷ˿�Ϊ4000����ʱ��IP��ַ����·�������䣬������Ϊ�̶�IP��
				SEND_DATA((u8 *)AT_CIPSTART1);    //���ӵ�TCP������ָ���˿�
				break;	
			case 70://                        "1234567890123456"
				
				esp_waitString(WAIT_OK,30,80);//WAIT_OK   WAIT_GOTIP  WAIT_READY	
				SEND_DATA((u8 *)AT_CIPMODE);    //����͸��ģʽ
				break;
			case 75://                        "1234567890123456"
				
				esp_waitString(WAIT_OK,30,85);//WAIT_OK   WAIT_GOTIP  WAIT_READY	
				SEND_DATA((u8 *)AT_CIPMODE);    //����͸��ģʽ
				break;
			case 80://                        "1234567890123456"
				ShowYWFFlag=3;
				ShowYWFFlags=2;//��ʾ�������
				esp_waitString(WAIT_OK,50,90);//WAIT_OK   WAIT_GOTIP  WAIT_READY	
				SEND_DATA((u8 *)AT_CIPSEND); //
			
				break;
			case 85://                        "1234567890123456"
				
				ShowYWFFlag=3;	
				ShowYWFFlags=2;
				esp_waitString(WAIT_OK,50,100);//WAIT_OK   WAIT_GOTIP  WAIT_READY	
				SEND_DATA((u8 *)AT_CIPSEND); //
			
				break;			
			case 90://                        "1234567890123456"	
				ShowYWFover=1;
				SEND_DATA((u8 *)"GET http://api.k780.com/?app=life.time&appkey=52153&sign=3cd060abec1719000c89ab9bca701f18&format=json\r\n"); //
				Count=0;
				esp_step=1;
				
				break;	
			case 100:
				ShowYWFover=2;
				SEND_DATA((u8 *)"GET http://gfapi.mlogcn.com/weather/v001/now?areacode=101260201&key=7fh8CTqIn0Z4l6kAMKPCjkxwrtdIQqMP\r\n"); //
				Count=0;
				esp_step=105;
				
				break;
			case 101://��������
				SEND_DATA((u8 *)"+++");
				Count=0;
				esp_step=103;
				break;
			case 102://����ʱ��
				SEND_DATA((u8 *)"+++");
				Count=0;
			WEEKS_COM();
				esp_step=104;
				break;
			case 103://��������
				esp_waitString(WAIT_OK,5,0);//WAIT_OK   WAIT_GOTIP  WAIT_READY	
				SEND_DATA((u8 *)AT); //��������ָ��(u8 *)"AT"
				
				esp_step=55;
				break;
			case 104://����ʱ��
				esp_waitString(WAIT_OK,5,0);//WAIT_OK   WAIT_GOTIP  WAIT_READY	
				SEND_DATA((u8 *)AT); //��������ָ��(u8 *)"AT"
				
				esp_step=54;
				break;
			case 105://���ũ��
			GetChinaCalendarStr((int )(2000+(SysDateTime.year)),
													(int )(SysDateTime.month),
													(int )(SysDateTime.date),
													(char *)showhs);	

			showhs[14]='\0';
					Count=0;
					ShowYWFover=3;
					ShowYWFFlag=0;
					ShowYWFFlags=0;
					esp_step=0;
				break;
			case 106://���ũ��
				GetChinaCalendarStr((int )(2000+(SysDateTime.year)),
													(int )(SysDateTime.month),
													(int )(SysDateTime.date),
													(char *)showhs);	

				showhs[14]='\0';
					Count=0;
				Count=0;
				esp_step=0;
			break;	
			//����251��252��253�ֱ�Ϊ����CLOSED������ERROR���ȴ���ʱ
			case 251:	//����CLOSED
				
				break;	
			
			case 252:	//����ERROR
				
				break;
			case 253:	//�ȴ���ʱ
				CLR_Buf();//�������
				esp_step=esp_stepSave;
			
				break;	
			//����254��255�ֱ�Ϊ��ʱ���ȴ��ض��ַ���
			case 254:	//��ʱ
				if(esp_delay)esp_delay--;
				else esp_step=esp_stepNext;
				break;		
			case 255://�ȴ��ض��ַ������磺OK��ready��GOT IP��	
		/*if(esp_cnt8!=5)esp_cnt8++;	
					else
					{
						esp_cnt8=0;*/
						//==100ms��0.1��=====
						if(esp_delay)esp_delay--;
						else
						{
							esp_step=253;	//�ȴ���ʱ
							//                          "1234567890123456"
						
						}
					//}
			/*	if(	((esp_waitflg&WAIT_OK)&&(strstr(Rx232buffer,"OK\r\n")!=NULL))  || 
						((esp_waitflg&WAIT_GOTIP)&&(strstr(Rx232buffer,"GOT IP\r\n")!=NULL))||
						((esp_waitflg&WAIT_READY)&&(strstr(Rx232buffer,"ready\r\n")!=NULL)) ||
						((strstr(Rx232buffer,"CONNECT")!=NULL)))
						{
								esp_step=esp_stepNext;
								CLR_Buf();//�������
						}*/
				if(	((esp_waitflg & WAIT_OK) && (strstr(Rx232buffer, "OK\r\n") != NULL))  ||
						((esp_waitflg & WAIT_GOTIP) && (strstr(Rx232buffer, "WIFI GOT IP\r\n") != NULL)) ||
						((esp_waitflg & WAIT_READY) && (strstr(Rx232buffer, "ready\r\n") != NULL)) 
						//||((esp_waitflg & WAIT_OVER) && (strstr(Rx232buffer, "cmd=1&res=1\r\n") != NULL))
						)//|| ((strstr(Rx232buffer, "CONNECT") != NULL))
            {
                esp_step = esp_stepNext;
                CLR_Buf();//�������
            }
				
				if(strstr(Rx232buffer,"ERROR")!=NULL)
				{
					esp_step=252;	//����ERROR
					//                              "1234567890123456"
	
				}
				
				break;		
			
		}	
		
	
}


