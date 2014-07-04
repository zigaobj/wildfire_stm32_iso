#ifndef __KEY_H
#define	__KEY_H

#include "stm32f10x.h"
 /*******
 *按键按下标置
 KEY_ON 0
 KEY_OFF 1
 ********/
#define KEY_ON	0
#define KEY_OFF	1

void Key1_GPIO_Config(void);
void Key2_GPIO_Config(void);
void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin);
void Key_Test(void);

#endif /* __LED_H */

