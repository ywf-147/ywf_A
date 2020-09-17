#include "main.h"         
#include "esp8266.h"

//===============================================

u8 test=0;
//u8 Count=0;  
u16 Count=0;  
// 接收数组累加计数       
char Rx232buffer[Buf_Max];                                         // 接收数组
u8 WorkMode=1;//工作模式

#define SET_ESP_STA    			0x80		//设置为STA模式
#define SET_ESP_AP     			0x40		//设置为AP模式
#define SET_ESP_AP_STA 			0x20		//设置为AP+STA模式
#define SET_ESP_GET_TIME		0x10		//设置获取时间
#define SET_ESP_GET_WEATHER	0x08		//设置获取天气

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
	esp_stepSave=esp_step;//保存当前步骤（出现等待超时或者返回ERROR时，便于知道是从哪个case分支进来的）
	esp_stepNext=next;	
	esp_waitflg=wait;
	esp_delay=delay;
	CLR_Buf();//清除缓存
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
 * 描  述 : 初始化ESP8266模块用到的引脚
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void esp8266_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  //打开所用GPIO
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 
  
  //配置的IO是PB10，串口3的TXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;           //复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  //配置的IO是PB11，串口3的RXD
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;     //浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
     
  //配置的IO是PB6 PB7，ESP8266复位引脚RST
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;                
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;           //推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
  //初始状态将ESP8266复位引脚使能置高
  ESP8266_RST_H;
	ESP8266_EN_H;
}

/**************************************************************************************
 * 描  述 : 初始化USART3并配置USART3中断优先级
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void USART3_Init(void)
{
	USART_InitTypeDef				USART_InitStructure;
	USART_ClockInitTypeDef  USART_ClockInitStructure;
	NVIC_InitTypeDef				NVIC_InitStructure;

  //打开串口3的时钟
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3 , ENABLE); 
	
  //配置串口3的硬件参数
	USART_DeInit(USART3);                                            //将外设USART3寄存器重设为缺省值
	USART_InitStructure.USART_BaudRate = 115200;                      //设置串口3波特率为115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;      //设置一个帧中传输数据位 
	USART_InitStructure.USART_StopBits = USART_StopBits_1;           //定义发送的停止位数目为1
	USART_InitStructure.USART_Parity = USART_Parity_No;              //奇偶失能
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //硬件流控制失能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                     //发送接收使能
	USART_Init(USART3, &USART_InitStructure);    

  USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;      //时钟低电平活动
  USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;            //设置SCLK引脚上时钟输出的极性为低电平
  USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;          //时钟第一个边沿进行数据捕获
  USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;  //最后一位数据的时钟脉冲不从SCLK输出
  USART_ClockInit(USART3, &USART_ClockInitStructure); 
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);                   //使能USART3接收中断
	USART_Cmd(USART3, ENABLE);                                       //使能USART3外设
	
  //配置串口3中断优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);                  //NVIC_Group:先占优先级2位，从优先级2位
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;	               //配置为串口3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;          //先占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	             //从优先级为3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                  //使能中断通道
	NVIC_Init(&NVIC_InitStructure);
	
}

/**************************************************************************************
 * 描  述 : USART3发送函数
 * 入  参 : 单个字符
 * 返回值 : 无
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
 * 描  述 : 握手成功与否函数
 * 入  参 : char *a待对比字符串
 * 返回值 : 无
 **************************************************************************************/
bool Hand( char *a)
{ 
  if(strstr(Rx232buffer,a)!=NULL)    //判断指针a中的字符串是否是Rx232buffer数组中字符串的子串
	   return true;
	else
		 return false;
}

/**************************************************************************************
 * 描  述 : 清空接收数组数据
 * 入  参 : 1为在配置模式中//清除缓存，2为工作模式中//清除缓存
 * 返回值 : 无
 **************************************************************************************/
void CLR_Buf(void)
{
		memset(Rx232buffer,0,Buf_Max);
		 Count = 0;                                         //接收数组累加计数清零，即下次接收从头开始
}


/**************************************************************************************
 * 描  述 : USART3全局中断服务
 * 入  参 : 无	WorkMode
 * 返回值 : 无
 **************************************************************************************/
void USART3_IRQHandler(void)
{  
	u8 tmp=0;
  if(USART_GetITStatus(USART3,USART_IT_RXNE)!= RESET)       //接收中断
  {
		tmp= USART_ReceiveData(USART3);  
		if(WorkMode==1){//处于配置模式中						
			Rx232buffer[Count]=tmp;
			Count++;                                                //不断累加接收数据                          
			if(Count>=Buf_Max)                                       //接收数大于定义接收数组最大个数时，从头开始放置数据
			{
				 Count = 0;
			} 		
		}
       //通过外设USART3接收数据		
    USART_ClearITPendingBit(USART3, USART_IT_RXNE);         //清除USART3的中断待处理位
  }  
}


void ESP8266_Connect(void)
{
		switch(esp_step)
		{
			case 0:	
				

				break;//esp_step为0表示关闭	
			case 1:	
				
				esp_step=101;
				break;//esp_s
			case 10://   硬件复位             "1234567890123456"
				//display_GB2312_string(1,1,(u8 *)"1. 硬件复位     ");
				ESP8266_RST_L;	
				//esp_delay5ms(100,11);//设置延时100*5ms，然后转case 11
				esp_delay5ms(10,11);//等待100ms，然后转11步骤（100ms是20*5ms得来的）
				break;
			case 11:
				ESP8266_RST_H;//将ESP8266复位一次，此操作必不可少	
				esp_waitString(WAIT_READY,10,20);//WAIT_OK   WAIT_GOTIP  WAIT_READY	
				//esp_step=20;
				break;	
			
			case 20://                        "1234567890123456"
				//display_GB2312_string(1,1,(u8 *)"2.发送AT        ");
				esp_waitString(WAIT_OK,5,30);//WAIT_OK   WAIT_GOTIP  WAIT_READY	
				SEND_DATA((u8 *)AT); //发送联机指令(u8 *)"AT"
				break;
			case 30://                        "1234567890123456"
				//display_GB2312_string(1,1,(u8 *)"3.设为STA模式   ");
				esp_waitString(WAIT_OK,5,40);//WAIT_OK   WAIT_GOTIP  WAIT_READY		
				SEND_DATA((u8 *)AT_CWMODE1); //设置ESP8266工作模式 CWMODE=3为AP+STATION模式 CWMODE=1为STA模式
				break;
			case 40://                        "1234567890123456"
				//display_GB2312_string(1,1,(u8 *)"4.重启模块(软启)");
				esp_waitString(WAIT_OK|WAIT_READY,10,45);//WAIT_OK、WAIT_GOTIP	
				SEND_DATA((u8 *)AT_RST);  //重启模块（软件复位）	
				break;
			
			case 45://                        "1234567890123456"
				//display_GB2312_string(1,1,(u8 *)"x.连接307WiFi   ");
				esp_waitString(WAIT_GOTIP,20,50);//WAIT_OK、WAIT_GOTIP、WAIT_READY	
				SEND_DATA((u8 *)AT_CWJAP_CUR);                     //发送联机指令(u8 *)"AT+CWJAP=\"307\",\"987654321.\""
				break;	
			
			case 50://                        "1234567890123456"
				
				esp_waitString(WAIT_OK,50,60);	//WAIT_OK   WAIT_GOTIP  WAIT_READY	
				SEND_DATA((u8 *)AT_CIPMUX0);  //设置ESP8266连接模式  CIPMUX=1多链接  CIPMUX=0单链接（透传）
				break;
			
			case 54://断开服务器连接为连接K780服务器做准备	路径为 54 60 70 80 90 
				esp_waitString(WAIT_OK,50,60);	//WAIT_OK   WAIT_GOTIP  WAIT_READY
				SEND_DATA((u8 *)AT_CIPCLOSE);//断开服务器连接
				break;
			
			case 55://断开服务器连接为连接高分服务器做准备 路径为55 65 75 85 100
				esp_waitString(WAIT_OK,50,65);	//WAIT_OK   WAIT_GOTIP  WAIT_READY
				SEND_DATA((u8 *)AT_CIPCLOSE);//断开服务器连接
				break;
			
			case 60://                        "1234567890123456"
				
				esp_waitString(WAIT_OK,80,70);//WAIT_OK   WAIT_GOTIP  WAIT_READY	
			//SEND_DATA((u8 *)"AT+CIPSERVER=1,4000\r\n");    //建立TCP服务器且开放端口为4000（此时的IP地址是有路由器分配，已设置为固定IP）
				SEND_DATA((u8 *)AT_CIPSTART);    //连接到TCP服务器指定端口
				break;
			
			case 65://   连接高分服务器      
				
				esp_waitString(WAIT_OK,80,75);//WAIT_OK   WAIT_GOTIP  WAIT_READY	
			//SEND_DATA((u8 *)"AT+CIPSERVER=1,4000\r\n");    //建立TCP服务器且开放端口为4000（此时的IP地址是有路由器分配，已设置为固定IP）
				SEND_DATA((u8 *)AT_CIPSTART1);    //连接到TCP服务器指定端口
				break;	
			case 70://                        "1234567890123456"
				
				esp_waitString(WAIT_OK,30,80);//WAIT_OK   WAIT_GOTIP  WAIT_READY	
				SEND_DATA((u8 *)AT_CIPMODE);    //开启透传模式
				break;
			case 75://                        "1234567890123456"
				
				esp_waitString(WAIT_OK,30,85);//WAIT_OK   WAIT_GOTIP  WAIT_READY	
				SEND_DATA((u8 *)AT_CIPMODE);    //开启透传模式
				break;
			case 80://                        "1234567890123456"
				ShowYWFFlag=3;
				ShowYWFFlags=2;//显示更新完成
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
			case 101://更新天气
				SEND_DATA((u8 *)"+++");
				Count=0;
				esp_step=103;
				break;
			case 102://更新时间
				SEND_DATA((u8 *)"+++");
				Count=0;
			WEEKS_COM();
				esp_step=104;
				break;
			case 103://更新天气
				esp_waitString(WAIT_OK,5,0);//WAIT_OK   WAIT_GOTIP  WAIT_READY	
				SEND_DATA((u8 *)AT); //发送联机指令(u8 *)"AT"
				
				esp_step=55;
				break;
			case 104://更新时间
				esp_waitString(WAIT_OK,5,0);//WAIT_OK   WAIT_GOTIP  WAIT_READY	
				SEND_DATA((u8 *)AT); //发送联机指令(u8 *)"AT"
				
				esp_step=54;
				break;
			case 105://算出农历
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
			case 106://算出农历
				GetChinaCalendarStr((int )(2000+(SysDateTime.year)),
													(int )(SysDateTime.month),
													(int )(SysDateTime.date),
													(char *)showhs);	

				showhs[14]='\0';
					Count=0;
				Count=0;
				esp_step=0;
			break;	
			//以下251、252、253分别为返回CLOSED、返回ERROR、等待超时
			case 251:	//返回CLOSED
				
				break;	
			
			case 252:	//返回ERROR
				
				break;
			case 253:	//等待超时
				CLR_Buf();//清除缓存
				esp_step=esp_stepSave;
			
				break;	
			//以下254、255分别为延时、等待特定字符串
			case 254:	//延时
				if(esp_delay)esp_delay--;
				else esp_step=esp_stepNext;
				break;		
			case 255://等待特定字符串，如：OK、ready、GOT IP等	
		/*if(esp_cnt8!=5)esp_cnt8++;	
					else
					{
						esp_cnt8=0;*/
						//==100ms即0.1秒=====
						if(esp_delay)esp_delay--;
						else
						{
							esp_step=253;	//等待超时
							//                          "1234567890123456"
						
						}
					//}
			/*	if(	((esp_waitflg&WAIT_OK)&&(strstr(Rx232buffer,"OK\r\n")!=NULL))  || 
						((esp_waitflg&WAIT_GOTIP)&&(strstr(Rx232buffer,"GOT IP\r\n")!=NULL))||
						((esp_waitflg&WAIT_READY)&&(strstr(Rx232buffer,"ready\r\n")!=NULL)) ||
						((strstr(Rx232buffer,"CONNECT")!=NULL)))
						{
								esp_step=esp_stepNext;
								CLR_Buf();//清除缓存
						}*/
				if(	((esp_waitflg & WAIT_OK) && (strstr(Rx232buffer, "OK\r\n") != NULL))  ||
						((esp_waitflg & WAIT_GOTIP) && (strstr(Rx232buffer, "WIFI GOT IP\r\n") != NULL)) ||
						((esp_waitflg & WAIT_READY) && (strstr(Rx232buffer, "ready\r\n") != NULL)) 
						//||((esp_waitflg & WAIT_OVER) && (strstr(Rx232buffer, "cmd=1&res=1\r\n") != NULL))
						)//|| ((strstr(Rx232buffer, "CONNECT") != NULL))
            {
                esp_step = esp_stepNext;
                CLR_Buf();//清除缓存
            }
				
				if(strstr(Rx232buffer,"ERROR")!=NULL)
				{
					esp_step=252;	//返回ERROR
					//                              "1234567890123456"
	
				}
				
				break;		
			
		}	
		
	
}


