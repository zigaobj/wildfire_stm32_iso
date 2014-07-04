#include "bsp_rda5807m.h"
#include "bsp_i2c_gpio.h"
#include "bsp_SysTick.h"
#include "bsp_usart1.h" 
#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_touch.h"
#include "bsp_ili9341_lcd.h"
#include <stdio.h>

//#define delayms(ms) Delay_us(ms)

#define RDA5807M_SLAVE_ADDRESS 0x20


TOUCH_EVEN touch_even = E_REFRESH;		//事件标志

static uint8_t volume = 0;						//音量
static uint16_t chan = 0;							//频点	

static void RDA5807_write_reg(void);
static void RDA5807_read_reg(uint8_t *reg_buf);
static void RDA5807_power(void);
static void RDA5807_FM_seek(void);

// RDA5807 寄存器  写
static uint8_t RDA_reg_data[8] =
{
   0xd0,0x00,  // 02H
   0x00,0x00,  // 03H
   0x00,0x40,  // 04H
   0x90,0x88,  // 05H
};

/**
  * @brief  RDA5807_write_reg 连续写寄存器
  * @param  无  
  * @retval 无
  */
static void RDA5807_write_reg(void)
{
	uint8_t i;
	uint8_t ucAck;
	
	i2c_Start();
	
	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	i2c_SendByte(RDA5807M_SLAVE_ADDRESS | I2C_WR);
	
	/* 检测ACK */
    ucAck = i2c_WaitAck();
	if (ucAck == 1)
	{
		goto err_quit;
	}
	
	// 寄存器连续写操作
	for(i=0; i<8; i++)
	{
		i2c_SendByte(RDA_reg_data[i]);
		
		/* 检测ACK */
		ucAck = i2c_WaitAck();
		if (ucAck == 1)
		{
			goto err_quit;
		}
	}	
err_quit:
	i2c_Stop();
}

/**
  * @brief  RDA5807_read_reg 连续读寄存器
  * @param  无  
  * @retval 无
  */
void RDA5807_read_reg(uint8_t *reg_buf)
{
	uint8_t i;
	uint8_t ucAck;

	i2c_Start();

	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
	i2c_SendByte(RDA5807M_SLAVE_ADDRESS | I2C_RD);

	/* 检测ACK */
	ucAck = i2c_WaitAck();
	if (ucAck == 1)
	{
		goto err_quit;
	}

	for (i = 0; i < 4; i++)
	{
		reg_buf[i] = i2c_ReadByte();
		if (i == 3)
		{
			i2c_NAck();
		}
		else
		{                        
			i2c_Ack();
		}
	}

err_quit:
	i2c_Stop();
}

/**
  * @brief  RDA5807_power 收音模块上电，复位
  * @param  无  
  * @retval 无
  */
static void RDA5807_power(void)
{
	 Delay_ms(50);
	
	 // 发送软件复位指令
   RDA_reg_data[0] = 0x00;
   RDA_reg_data[1] = 0x02;
   RDA5807_write_reg();

   Delay_ms(10);

   // 收音模块默认参数
   RDA_reg_data[0] = 0xd0;
   RDA_reg_data[1] = 0x01;
   RDA5807_write_reg();
}

/**
  * @brief  RDA5807_FM_seek 自动寻台
  * @param  无  
  * @retval 无
  */
static void RDA5807_FM_seek(void)
{
   
   uint8_t  reg_data[4] = {0x00, 0x00, 0x00, 0x00};

   RDA_reg_data[3] &= ~(1 << 4);      //调谐禁用

   // 内部自动寻台使能
   RDA_reg_data[0] |=  (1 << 0);      //SEEK位置1
   RDA5807_write_reg();

    // 等待STC 标志置位
   while(0 == (reg_data[0] & 0x40))
   {
     Delay_ms(40);
      // 读取内部状态
     RDA5807_read_reg(reg_data);
	 
   }
    // 获取当前工作频点
   chan = reg_data[0] & 0x03;
   chan = reg_data[1] | (chan << 8);
   chan = chan << 6;
   
   printf("\r\n 寻台成功 \r\n");
   printf("当前工作频点为：%d" , chan);

    // 保存当前工作频点
   RDA_reg_data[2] = (chan >> 8) & 0xff;
   RDA_reg_data[3] = (chan & 0xff);
}

/**
  * @brief  RDA5807_FM_Test 收音机模块测试
  * @param  无  
  * @retval 无
  */
void RDA5807_FM_Test(void)
{
  /* config the led */
	LED1_ON;
	LED2_ON;	

	printf("\r\n rda5807m 测试实验 \r\n");
	
	if ( i2c_CheckDevice(RDA5807M_SLAVE_ADDRESS) == 0 )
	{
		printf("\r\n rda5807m 检测成功 \r\n");
	}
	else
	{
		printf("\r\n rda5807m 检测不成功 \r\n");
	}
	
	/* 上电复位 */
	RDA5807_power();
	
	printf("\r\n 上电成功 \r\n");
	
	/* 设置初始音量 */
	RDA5807_Volume_Set(5);

	/* 搜寻电台 */
	RDA5807_Seek_Next();
	
	while(1)
	{	
		if( Key_Scan(GPIOC,GPIO_Pin_13) == KEY_ON  )	//扫描按键
		{			
			touch_even = E_SEEK_NEXT;			              // 向后寻台
			/*反转*/
			LED1_TOGGLE;
		}  

		if( Key_Scan(GPIOA,GPIO_Pin_0) == KEY_ON  )
		{			
			touch_even = E_SEEK_PREV;                   // 向前寻台			
			/*反转*/
			LED1_TOGGLE;
		}
		
		/* 处理事件 */
		even_process();		
  }
}

/**
  * @brief  设置音量值
  * @param  vol:	0~15  
  * @retval 无
  */

void RDA5807_Volume_Set(uint8_t vol)
{
	volume = vol ;
	RDA_reg_data[7] = (RDA_reg_data[7] & 0xF0)|(volume & 0x0F);			// 05H寄存器低四bit 改变音量
	RDA5807_write_reg();
	printf("当前音量为%d" , volume);

}

/**
  * @brief  音量加
  * @param  无  
  * @retval 无
  */
void RDA5807_Volume_Add(void)
{
	if(volume>= 0x0F)
		volume=0x0F;
	else
		volume++;
	
	RDA_reg_data[7] = (RDA_reg_data[7] & 0xF0)|(volume & 0x0F);			// 05H寄存器低四bit 改变音量
	RDA5807_write_reg();
	printf("当前音量为%d" , volume);
}


/**
  * @brief  音量减
  * @param  无  
  * @retval 无
  */
void RDA5807_Volume_Dec(void)
{
	if(volume<= 0)
		volume=0;
	else
		volume--;
	
	RDA_reg_data[7] = (RDA_reg_data[7] & 0xF0)|(volume & 0x0F);			// 05H寄存器低四bit 改变音量
	RDA5807_write_reg();
	printf("当前音量为%d" , volume);
}

/**
  * @brief  向下寻台 seek up （频点增加）
  * @param  无  
  * @retval 无
  */
void RDA5807_Seek_Next(void)
{
	RDA_reg_data[0] |= (1 << 1); 	     //向下搜台	
	RDA5807_FM_seek();                 //自动寻台
}

/**
  * @brief  向上寻台 seek down（频点减小）
  * @param  无  
  * @retval 无
  */
void RDA5807_Seek_Prev(void)
{
	RDA_reg_data[0] &= ~(1 << 1); 	    //向上搜台
	RDA5807_FM_seek();                  //自动寻台
}


/**
  * @brief  处理事件
  * @param  无  
  * @retval 无
  */
void even_process(void)
{
	char lcd_char[20];
	
	switch(touch_even)
	{
		case E_VOL_ADD:
			RDA5807_Volume_Add();			
		
			Lcd_GramScan( 1 );
			sprintf(lcd_char,"音量:%2d",volume);		//显示音量到LCD上
			LCD_DispEnCh(20,200,lcd_char,BLUE);
		
			touch_even = E_NULL;
		
			break;
		
		case E_VOL_DEC:
			RDA5807_Volume_Dec();
			
		
			Lcd_GramScan( 1 );
			sprintf(lcd_char,"音量:%2d",volume);		//显示音量到LCD上
			LCD_DispEnCh(20,200,lcd_char,BLUE);
		
			touch_even = E_NULL;
		
			break;
				
		case E_SEEK_PREV:													//向上寻台
			RDA5807_Seek_Prev();
		
			Lcd_GramScan( 1 );
			sprintf(lcd_char,"工作频点:%6d",chan);	//显示频点到LCD上
			LCD_DispEnCh(20,150,lcd_char,BLUE);
		
			touch_even = E_NULL;
		
			break;
		
		case E_SEEK_NEXT:													//向下寻台			
			RDA5807_Seek_Next();
		
			Lcd_GramScan( 1 );
			sprintf(lcd_char,"工作频点:%6d",chan);	//显示频点到LCD上
			LCD_DispEnCh(20,150,lcd_char,BLUE);
		
			touch_even = E_NULL;
		
			break;
		
		case E_REFRESH:														//刷新屏幕		
			Lcd_GramScan( 1 );
			sprintf(lcd_char,"工作频点:%6d",chan);	//显示工作频点
			LCD_DispEnCh(20,150,lcd_char,BLUE);
		
			sprintf(lcd_char,"音量:%2d",volume);		//显示音量到LCD上
			LCD_DispEnCh(20,200,lcd_char,BLUE);
		
			touch_even = E_NULL;
		
		default:
			break;
	
		}

}

/**
  * @brief  touch_process中断服务函数，根据触摸位置更新触摸标志
  * @param  无  
  * @retval 无
  */
void touch_process (void)
{
		 if(touch_flag == 1)															/*如果触笔按下了*/
    {		
      /* 获取点的坐标 */
      if(Get_touch_point(&display, Read_2046_2(), &touch_para ) !=DISABLE)      
      {		
        LED1_TOGGLE;   																//LED反转	 
			//	printf("\r\n x=%d,y=%d",display.x,display.y);
				
				if(display.x>=46 && display.x<=78)						//控制条的范围
				{
						if(display.y>=18 && display.y<=73)				//向上寻台
						{
								touch_even = E_SEEK_PREV;
								printf("\r\n prev");
							}
						else if(display.y>=84 && display.y<=139)	//向下寻台	
						{
								touch_even = E_SEEK_NEXT;
								printf("\r\n next");
						}
						else if(display.y>=156 && display.y<=181)	//音量加
						{
								touch_even = E_VOL_ADD;
								printf("\r\n vol add");
						}
						else if(display.y>=185 && display.y<=210)	//音量减
						{
								touch_even = E_VOL_DEC;
								printf("\r\n vol dec");
						}
				}
			}				
		}
}

/*********************************************END OF FILE**********************/

