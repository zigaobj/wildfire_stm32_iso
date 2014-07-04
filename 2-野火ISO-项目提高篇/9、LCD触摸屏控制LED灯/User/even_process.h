#ifndef __EVEN_PROCESS_H
#define __EVEN_PROCESS_H

#include "stm32f10x.h"

typedef enum{E_NULL,E_BUTTON1,E_BUTTON2,E_BUTTON3,E_GETCHAR}EVEN;
						//空事件，按钮1，按钮2，按钮3，串口数据

typedef enum{S_ON,S_OFF}BUTTON_STATE;
						//状态开，状态关

void even_process(void);
void touch_process(void);

#endif


