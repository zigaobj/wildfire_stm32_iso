#ifndef __APP_UI_H
#define __APP_UI_H

#include "stm32f10x.h"

#define LCD_DISPLAY
//#define WORD_MODE

/* 实验测试状态标志   */
typedef enum {SB_NULL,SB_RUN,SB_MP3,SB_FM}TESTING_STATE;
//准备、播放、暂停、切歌


/* 触摸事件标志 */
typedef enum {EB_NULL,EB_SELT_PREV,EB_SELT_NEXT,EB_SELECT,EB_RESET}TOUCH_EVEN_BOARDTEST;
//空事件、播放暂停事件、上一首、下一首、音量+、音量减、直接选择歌曲、向上选择、向下选择、上一页、下一页、选择OK、选择扬声器、


void Lcd_List(char page);
void even_process_boardtest(void);
void Lcd_Note(uint8_t test_num);
void Lcd_Touch(uint8_t test_num);

#endif /* __APP_UI_H */

