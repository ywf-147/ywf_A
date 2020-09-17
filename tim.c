#include "main.h"
#include "tim.h"
u8 flag1ms = 0;
u8 flag5ms = 0;
u8 flag200ms = 0;
u8 flag500ms = 0;
u8 flag1s = 0;

//��ʱ��ʱ����㹫ʽTout = ((��װ��ֵ+1)*(Ԥ��Ƶϵ��+1))/ʱ��Ƶ��;
//���磺1�붨ʱ����װ��ֵ=9999��Ԥ��Ƶϵ��=7199

void TIM3_Init(u16 arr, u16 psc)  //TIM3 ��ʼ�� arr��װ��ֵ pscԤ��Ƶϵ��
{
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseInitStrue;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʹ��TIM3
    TIM3_NVIC_Init (); //����TIM3�ж�����

    TIM_TimeBaseInitStrue.TIM_Period = arr; //�����Զ���װ��ֵ
    TIM_TimeBaseInitStrue.TIM_Prescaler = psc; //Ԥ��Ƶϵ��
    TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up; //�������������
    TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1; //ʱ�ӵķ�Ƶ���ӣ�����һ������ʱ���ã�һ����ΪTIM_CKD_DIV1
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStrue); //TIM3��ʼ������
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//ʹ��TIM3�ж�
    TIM_Cmd(TIM3, ENABLE); //ʹ��TIM3
}

void TIM3_NVIC_Init (void)  //����TIM3�ж�����
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x3;	//������ռ�������ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void TIM3_IRQHandler(void)  //TIM3�жϴ�����
{
    static	u16 cnt = 0, cnt200ms = 0, cnt5ms = 0;

    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 	//�ж��Ƿ���TIM3�ж�
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
//�˴�д���û��Լ��Ĵ������
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

//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM1_PWM_Init(u16 arr, u16 psc)
{
    //�˲������ֶ��޸�IO������
    RCC->APB2ENR |= 1 << 11;       	//TIM1ʱ��ʹ��

    RCC->APB2ENR |= 1 << 2;
    GPIOA->CRH &= 0XFFFFFF00;			//PA8PA9���
    GPIOA->CRH |= 0X000000BB;			//�����������
    GPIOA->ODR |= 3 << 8;						//PA8PA9�ø�


    TIM1->ARR = arr;							//�趨�������Զ���װֵ
    TIM1->PSC = psc;							//Ԥ��Ƶ������Ƶ

    TIM1->CCMR1 |= 6 << 4;  				//CH1 ����Ƚ�2ģʽ PWM2ģʽ
    TIM1->CCMR1 |= 1 << 3; 					//CH1 ����Ƚ�2ģʽ Ԥװ��ʹ��

    TIM1->CCMR1 |= 7 << 12;  				//CH2 ����Ƚ�2ģʽ PWM2ģʽ
    TIM1->CCMR1 |= 1 << 11; 				//CH2 ����Ƚ�2ģʽ Ԥװ��ʹ��

    TIM1->CCER |= 1 << 0;   				//OC2 ���ʹ��
    TIM1->CCER |= 3 << 4;   				//OC2 ���ʹ��

    TIM1->CR1 |= 1 << 7;   					//ARPEʹ��
    TIM1->CR1 |= 0 << 4;    				//���¼���ģʽ
    TIM1->BDTR |= 1 << 15;

//	TIM1->EGR|=1<<10;						//��ʼ�����мĴ���
    TIM1->CR1 |= 1 << 0;						//ʹ�ܶ�ʱ��1

}





