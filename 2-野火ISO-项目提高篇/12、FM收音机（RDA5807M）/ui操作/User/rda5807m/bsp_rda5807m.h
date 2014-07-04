#ifndef __RDA5807M_H
#define	__RDA5807M_H

#include "stm32f10x.h"


typedef enum{E_NULL,E_VOL_ADD,E_VOL_DEC,E_SEEK_PREV,E_SEEK_NEXT,E_REFRESH}TOUCH_EVEN;
						//空事件，音量加，音量减，向上寻台，向下寻台，刷新屏幕
void RDA5807_Volume_Set(uint8_t vol);
void RDA5807_Volume_Add(void);
void RDA5807_Volume_Dec(void);
void RDA5807_Seek_Prev(void);
void RDA5807_Seek_Next(void);
void RDA5807_FM_Test(void);
void even_process(void);

#endif /* __RDA5807M_H */


