#include "main.h"
#include "tim.h"
u8 flag1ms = 0;
u8 flag5ms = 0;
u8 flag200ms = 0;
u8 flag500ms = 0;
u8 flag1s = 0;

//定时器时间计算公式Tout = ((重装载值+1)*(预分频系数+1))/时钟频率;
//例如：1秒定时，重装载值=9999，预分频系数=7199

void TIM3_Init(u16 arr, u16 psc)  //TIM3 初始化 arr重装载值 psc预分频系数
{
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseInitStrue;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //使能TIM3
    TIM3_NVIC_Init (); //开启TIM3中断向量

    TIM_TimeBaseInitStrue.TIM_Period = arr; //设置自动重装载值
    TIM_TimeBaseInitStrue.TIM_Prescaler = psc; //预分频系数
    TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up; //计数器向上溢出
    TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1; //时钟的分频因子，起到了一点点的延时作用，一般设为TIM_CKD_DIV1
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStrue); //TIM3初始化设置
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//使能TIM3中断
    TIM_Cmd(TIM3, ENABLE); //使能TIM3
}

void TIM3_NVIC_Init (void)  //开启TIM3中断向量
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x3;	//设置抢占和子优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM3_IRQHandler(void)  //TIM3中断处理函数
{
    static	u16 cnt = 0, cnt200ms = 0, cnt5ms = 0;

    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 	//判断是否是TIM3中断
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
//此处写入用户自己的处理程序
        flag1ms = 1;

        if(cnt5ms != 4)cnt5ms++;
        else
        {
            cnt5ms = 0;
            flag5ms = 1;
        }

        if(cnt200ms != 199)cnt200ms++;
        else
        {
            cnt200ms = 0;
            flag200ms = 1;
        }

        if(cnt != 999)cnt++;
        else
        {
            cnt = 0;
            flag1s = 1;
            LED1 = ~LED1;
        }
    }
}

//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM1_PWM_Init(u16 arr, u16 psc)
{
    //此部分需手动修改IO口设置
    RCC->APB2ENR |= 1 << 11;       	//TIM1时钟使能

    RCC->APB2ENR |= 1 << 2;
    GPIOA->CRH &= 0XFFFFFF00;			//PA8PA9输出
    GPIOA->CRH |= 0X000000BB;			//复用推挽输出
    GPIOA->ODR |= 3 << 8;						//PA8PA9置高


    TIM1->ARR = arr;							//设定计数器自动重装值
    TIM1->PSC = psc;							//预分频器不分频

    TIM1->CCMR1 |= 6 << 4;  				//CH1 输出比较2模式 PWM2模式
    TIM1->CCMR1 |= 1 << 3; 					//CH1 输出比较2模式 预装载使能

    TIM1->CCMR1 |= 7 << 12;  				//CH2 输出比较2模式 PWM2模式
    TIM1->CCMR1 |= 1 << 11; 				//CH2 输出比较2模式 预装载使能

    TIM1->CCER |= 1 << 0;   				//OC2 输出使能
    TIM1->CCER |= 3 << 4;   				//OC2 输出使能

    TIM1->CR1 |= 1 << 7;   					//ARPE使能
    TIM1->CR1 |= 0 << 4;    				//向下计数模式
    TIM1->BDTR |= 1 << 15;

//	TIM1->EGR|=1<<10;						//初始化所有寄存器
    TIM1->CR1 |= 1 << 0;						//使能定时器1

}





