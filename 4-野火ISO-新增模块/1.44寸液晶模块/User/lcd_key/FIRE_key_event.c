
#include "FIRE_key_event.h"
#include "bsp_led.h"
#include "FIRE_lcd.h"



void key_event_init(void)
{
    //初始化 全部 按键
    key_init();
	
		/* 初始化其它处理函数，例如按键修改变量等 */
}

void deal_key_event(void)
{
    KEY_MSG_t keymsg;
		Site_t site = {20,20};
		Size_t size = {128,16};

    while(get_key_msg(& keymsg))     //获得按键就进行处理
    {
        if(keymsg.status == KEY_DOWN)
        {
            switch(keymsg.key)
            {
            case KEY_U:							//可以根据自己需求进行不同的功能处理，这里作为范例，显示不同信息到lcd上
								LCD_rectangle(site,size,RED);
                LCD_str(site,"KEY_U DOWN",BLUE,RED);
								LED1_TOGGLE;
							
                break;

            case KEY_D:
                LCD_rectangle(site,size,RED);
                LCD_str(site,"KEY_D DOWN",BLUE,RED);
								LED1_TOGGLE;
							
                break;

            case KEY_L:
                LCD_rectangle(site,size,RED);
                LCD_str(site,"KEY_L DOWN",BLUE,RED);
								LED1_TOGGLE;
							
                break;

            case KEY_R:
                LCD_rectangle(site,size,RED);
                LCD_str(site,"KEY_R DOWN",BLUE,RED);
								LED1_TOGGLE;
							
                break;

            case KEY_B:
                LCD_rectangle(site,size,RED);
                LCD_str(site,"KEY_B DOWN",BLUE,RED);
								LED1_TOGGLE;
						
                break;

//            case KEY_A:
//                val_cancel();
//               break;
            default:
                break;
            }
        }
        else if(keymsg.status == KEY_HOLD)
        {
            switch(keymsg.key)
            {
            case KEY_U:
                LCD_rectangle(site,size,RED);
                LCD_str(site,"KEY_U HOLD",BLUE,RED);
							
								LED3_TOGGLE;
                break;

            case KEY_D:
                LCD_rectangle(site,size,RED);
                LCD_str(site,"KEY_D HOLD",BLUE,RED);
								
								LED3_TOGGLE;
                break;

            case KEY_L:
                LCD_rectangle(site,size,RED);
                LCD_str(site,"KEY_L HOLD",BLUE,RED);
								
								LED3_TOGGLE;
                break;

            case KEY_R:
                LCD_rectangle(site,size,RED);
                LCD_str(site,"KEY_R HOLD",BLUE,RED);
								
								LED3_TOGGLE;
                break;

            case KEY_B:                //长按 OK 键
                LCD_rectangle(site,size,RED);
                LCD_str(site,"KEY_B HOLD",BLUE,RED);
							
								LED3_TOGGLE;

            default:            //不需要处理 cancel
                break;
            }
        }
        else
        {
            //KEY_UP ,不进行处理
        }
    }
}





