#ifndef __IRDA_H
#define	__IRDA_H

#include "stm32f10x.h"
void EXTI_PB11_Config(void);
void IrDa_Init(void);
uint8_t Get_Pulse_Time(void);
uint8_t IrDa_Process(void);

#define IRDA_ID 0

 //读取引脚的电平
#define  IrDa_DATA_IN()	   GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)

#endif /* __IRDA_H */
