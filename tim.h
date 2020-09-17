#ifndef  __TIM_H
#define  __TIM_H
#include "sys.h"

extern u8 flag1s;
extern u8 flag200ms;
extern u8 flag500ms;
extern u8 flag5ms;
extern u8 flag1ms;

#define TIM1_PWM1_VAL TIM1->CCR1
#define TIM1_PWM2_VAL TIM1->CCR2
void TIM3_Init(u16 arr, u16 psc);
void TIM3_NVIC_Init (void);

void TIM1_PWM_Init(u16 arr, u16 psc);
#endif
