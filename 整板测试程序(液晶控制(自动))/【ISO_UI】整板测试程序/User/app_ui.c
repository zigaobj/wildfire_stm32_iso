#include <stdio.h>
#include <string.h>
#include "app_ui.h"

#include "ff.h"
#include "diskio.h"
#include "bsp_led.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_bmp.h"
#include "bsp_touch.h"

#define TEST_NUM			17
#ifdef WORD_MODE
#define PER_PAGE			8					//每页显示的数目	

#else

#define PER_PAGE			6					//每页显示的数目	
#endif


uint8_t current_page_boardtest=1;
uint8_t all_page_boardtest = (TEST_NUM+(PER_PAGE-1))/PER_PAGE ;		//每页显示8个实验,+(PER_PAGE-1)是因为使用进一法


uint8_t select_index_boardtest=0;								//选择索引	
//uint8_t testing_index=0;							//正在测试的程序索引

TESTING_STATE testing_state=SB_NULL;			//播放器状态标志位，指示切歌，正在播放，暂停等状态
TOUCH_EVEN_BOARDTEST touch_even_boardtest;					//触摸状态标志位，指示触摸到播放器的哪一个按钮，如音量+-等

// iso 整板测试
extern volatile uint8_t rec_cmd;
extern void Soft_Reset(void);


/*实验名字，用于显示*/
uint8_t test_name[TEST_NUM][24]={
	"流水灯测试",												//1
	"按键测试",													//2
	"EEPROM测试",												//3
	"RTC万年历",						  			//4
	"ADC测试",											//5
	"FM收音机",											//6
	"MP3-helix解码",	//7
	"CAN(Loopback)",	//8						
	"DHT11温湿度",									//9
	"DS18b20温度传感器",						//10
	"Usb-Mass-Storage测试",	//11
	"摄像头(火眼ov7725)",						//12											
	"以太网ENC28J60",								//13
	"呼吸灯测试",										//14
	"SRAM测试",													//15
	"NORFLASH测试",											//16
	"NANDFLASH测试",										//17
	

};

static void delay(uint16_t t)
{
	uint8_t i;
	while(t--)
	{
		for(i=0;i<250;i++);
	}
}


/**
  * @brief  lcd_list 显示实验列表
  * @param  实验列表的页码
  * @retval none
  */

 void Lcd_List(char page)
{	

#ifdef WORD_MODE				//文字版
	char i;
	char lcd_char[28];												//+4是为了预留空间的实验编号
	
	
	select_index_boardtest=0;														//重新显示新的一页时重置select_index
	Lcd_GramScan( 1 );												//设置扫描方向
	LCD_Clear(12,86,206,145,BACKGROUND);
	
	
	sprintf(lcd_char,"第 %0d/%0d 页",current_page_boardtest,all_page_boardtest);		//显示页码
	LCD_DispEnCh(30,235,(const uint8_t *)lcd_char,BLUE);
		
	/* 打开playlist，读取音频文件名 */
	for(i=0;(i+PER_PAGE*(page-1))<TEST_NUM && i< PER_PAGE;i++)								//仅显示第一页
	{	
	
		sprintf(lcd_char,"%0d.%s",PER_PAGE*(page-1)+i+1,test_name[(PER_PAGE*(page-1) + i)]);			//拼接出歌曲列表
		
		LCD_DispEnCh(22,87+i*18,(const uint8_t *)lcd_char,BLACK);										//显示

	}	
	
#else			//图标版
	char i;
	char lcd_char[28];												//+4是为了预留空间的实验编号
	char pic_name[50];
	
	select_index_boardtest=0;														//重新显示新的一页时重置select_index
	Lcd_GramScan( 1 );												//设置扫描方向
	LCD_Clear(12,94,212,160,BACKGROUND);//BLUE BACKGROUND
	
	
	sprintf(lcd_char,"第 %0d/%0d 页",current_page_boardtest,all_page_boardtest);		//显示页码
	LCD_DispEnCh(30,235,(const uint8_t *)lcd_char,BLUE);
		
	/* 打开playlist，读取音频文件名 */
	for(i=0;(i+PER_PAGE*(page-1))<TEST_NUM && i< PER_PAGE;i++)								//仅显示第一页
	{		
		sprintf(pic_name,"/boardtest/ISO/ui_test%d.bmp",PER_PAGE*(page-1)+i+1);

		if(i<=((PER_PAGE/2)-1))
		{
				Lcd_show_bmp(160, 166-(70*i),pic_name);
		}
		else
		{
				Lcd_show_bmp(91, 166-(70*(i-(PER_PAGE/2))),pic_name);

			}
	}		
	
#endif

}


/*触摸时显示触摸效果*/
void Lcd_Touch(uint8_t test_num)
{
	char i = ((test_num-1)%PER_PAGE);
	char pic_name[50];	
	
		sprintf(pic_name,"/boardtest/ISO/ui_tch_test%d.bmp",test_num);

		if(i<=((PER_PAGE/2)-1))
		{
			Lcd_show_bmp(160, 166-(70*i),pic_name);				
		}
		else
		{
			Lcd_show_bmp(91, 166-(70*(i-(PER_PAGE/2))),pic_name);
		}
		
		
		delay(0x2FFf);
		sprintf(pic_name,"/boardtest/ISO/ui_test%d.bmp",test_num);
		
		if(i<=((PER_PAGE/2)-1))
		{
			Lcd_show_bmp(160, 166-(70*i),pic_name);				
		}
		else
		{
			Lcd_show_bmp(91, 166-(70*(i-(PER_PAGE/2))),pic_name);
		}
		delay(0xFFF);

	


}



void Lcd_Note(uint8_t test_num)
{
	uint8_t lcd_char[50];
	Lcd_GramScan( 1 );												//设置扫描方向
	LCD_Clear(12,86,206,165,BACKGROUND);			//清除屏幕
	
//	LCD_Clear(12,72,215,152,BACKGROUND);							//RED	 清除				
	//LCD_DispEnCh(50,90,test_name[test_num-1],RED);	
	
	sprintf(lcd_char,"%s程序运行中:",test_name[test_num-1]);

	LCD_DispEnCh(15,90,lcd_char,BLACK);		
	
	
	LCD_DispEnCh(10,230,"软件或硬件复位返回选择界面",BLACK);	


}
/**
  * @brief  touch_process 触摸事件处理，在触摸中断时被调用
  * @param  none
  * @retval none
  */
void touch_process_boardtest (void)
{
	
	  if(touch_flag == 1)			/*如果触笔按下了*/
    {
		
      /* 获取点的坐标 */
      if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)      
      {		
        LED1_TOGGLE;   																//LED反转	    
				printf("\r\nx=%d,y=%d",display.x,display.y);
				//控制条的x范围，再根据y坐标区分按键
				if(display.x>=6 && display.x<=38)							
				{
						if(display.y >=13 &&display.y<=35)		//上一页
						{
							touch_even_boardtest = EB_SELT_PREV;									
							printf("\r\n 上一页");
						}
						else if(display.y >=48 &&display.y<=80)	//下一页
						{										
							touch_even_boardtest = EB_SELT_NEXT;	
							printf("\r\n 下一页");
						}
						else if(display.y >=180 &&display.y<=218)	//复位
						{
							touch_even_boardtest = EB_RESET;
							printf("\r\n 复位");
						}
							
				}
				
		#ifdef WORD_MODE	//文字模式
					
				//直接选择实验，第1.2.3.4.。。。8~
					
				else if(display.x>208 && display.x<226)
				{
					touch_even_boardtest = EB_SELECT;	
					select_index_boardtest = 1;
					printf("\r\n rec_cmd=%d",select_index_boardtest);
				}
				else if(display.x>190 && display.x<208)
				{
					touch_even_boardtest = EB_SELECT;	
					select_index_boardtest = 2;
					printf("\r\n select_index_boardtest=%d",select_index_boardtest);
				}
				else if(display.x>172 && display.x<190)
				{
					touch_even_boardtest = EB_SELECT;	
					select_index_boardtest = 3;
					printf("\r\n select_index_boardtest=%d",select_index_boardtest);
				}
				else if(display.x>154 && display.x<172)
				{
					touch_even_boardtest = EB_SELECT;	
					select_index_boardtest = 4;
					printf("\r\n select_index_boardtest=%d",select_index_boardtest);
				}
				else if(display.x>136 && display.x<154)
				{
					touch_even_boardtest = EB_SELECT;
					select_index_boardtest = 5;
					printf("\r\n select_index_boardtest=%d",select_index_boardtest);
				}
				else if(display.x>118 && display.x<136)
				{
					touch_even_boardtest = EB_SELECT;	
					select_index_boardtest = 6;
					printf("\r\n select_index_boardtest=%d",select_index_boardtest);
				}
				else if(display.x>100 && display.x<118)
				{
					touch_even_boardtest = EB_SELECT;	
					select_index_boardtest = 7;
					printf("\r\n select_index_boardtest=%d",select_index_boardtest);
				}
				else if(display.x>82 && display.x<100)
				{
					touch_even_boardtest = EB_SELECT;	
					select_index_boardtest = 8;
					printf("\r\n select_index_boardtest=%d",select_index_boardtest);
				}
		#else			//图标模式
				
				else if(display.x>=140 && display.x<=195)			//第一行图标					
				{
						if(display.y>17 && display.y<70)
						{
							touch_even_boardtest = EB_SELECT;	
							select_index_boardtest = 1;
							printf("\r\n rec_cmd=%d",select_index_boardtest);
						}
						else if(display.y>86 && display.y<144)
						{
							touch_even_boardtest = EB_SELECT;	
							select_index_boardtest = 2;
							printf("\r\n select_index_boardtest=%d",select_index_boardtest);
						}
						else if(display.y>163 && display.y<210)
						{
							touch_even_boardtest = EB_SELECT;	
							select_index_boardtest = 3;
							printf("\r\n select_index_boardtest=%d",select_index_boardtest);
						}

				}
					
				else if(display.x>=70 && display.x<=125)			//第一行图标					
				{
						if(display.y>17 && display.y<70)
						{
							touch_even_boardtest = EB_SELECT;	
							select_index_boardtest = 4;
							printf("\r\n select_index_boardtest=%d",select_index_boardtest);
						}
						else if(display.y>86 && display.y<144)
						{
							touch_even_boardtest = EB_SELECT;	
							select_index_boardtest = 5;
							printf("\r\n select_index_boardtest=%d",select_index_boardtest);
						}
						else if(display.y>163 && display.y<210)
						{
							touch_even_boardtest = EB_SELECT;	
							select_index_boardtest = 6;
							printf("\r\n select_index_boardtest=%d",select_index_boardtest);
						}

				}							

				
		#endif
						
      }//if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)      
			
    }// if(touch_flag == 1)			
	
}





/**
  * @brief  even_process 根据事件标志进行处理
  * @param  none
  * @retval none
  */
void even_process_boardtest(void)
{
	static uint8_t flag = 0;
	switch(touch_even_boardtest)
	{
		
		/* 播放、暂停键	*/
		case EB_RESET:												
		
			Soft_Reset();
				
			touch_even_boardtest = EB_NULL;
			break;	
				
		/* 直接点选歌曲	*/
		case EB_SELECT:												
			
			rec_cmd = select_index_boardtest + ((current_page_boardtest-1)*PER_PAGE);											//根据当前页和select_index_boardtest确定rec_cmd
			if(flag == 0)	//第一次按下
			{
				Lcd_Touch(rec_cmd);
				flag = 1;	
			}
			testing_state = SB_RUN;
		
			touch_even_boardtest = EB_NULL;	
		
			break;			
		
		/* 下一页				*/
		case EB_SELT_NEXT:		
			if(testing_state == SB_RUN)
				Soft_Reset();
			
			if(current_page_boardtest<all_page_boardtest)
			{
				current_page_boardtest++;				
				Lcd_List(current_page_boardtest);						//刷新LCD列表
			}
			else
				current_page_boardtest =all_page_boardtest;
		
			touch_even_boardtest = EB_NULL;
			
			break;
		
		/* 上一页			*/
		case EB_SELT_PREV:	
			if(testing_state == SB_RUN)
				Soft_Reset();
		
			if(current_page_boardtest>1)					
			{			
				current_page_boardtest--;										//更新当前页码
				Lcd_List(current_page_boardtest);						//刷新LCD列表
			}
			else
				current_page_boardtest =1;
			
			touch_even_boardtest = EB_NULL;
			break;		
		
		default:
			//touch_even_boardtest = EB_NULL;
			break;
	}

}



