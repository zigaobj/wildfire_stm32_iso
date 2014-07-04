/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2013,野火科技
 *     All rights reserved.
 *     技术讨论：野火初学论坛 http://www.chuxue123.com
 *
 *     除注明出处外，以下所有内容版权均属野火科技所有，未经允许，不得用于商业用途，
 *     修改内容时必须保留野火科技的版权声明。
 *
 * @file       FIRE_LCD_ST7735S.c
 * @brief      LCD 函数库
 * @author     野火科技
 * @version    v5.0
 * @date       2013-09-07
 */
 
#include "bsp_lcd_st7735s.h"


//定义所用到的 IO管脚

#define ST7735S_H       128
#define ST7735S_W       128

uint16_t st7735s_h   = ST7735S_H;
uint16_t st7735s_w   = ST7735S_W;
uint8_t  st7735s_dir = ST7735S_DIR_DEFAULT;



static void Lcd_Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}	
#if 0

void LCD_ST7735S_wr_data(uint8_t data)
{
    LCD_ST7735S_RD_H;
    LCD_ST7735S_RS_H;
    LCD_ST7735S_CS_L;

    LCD_ST7735S_P0(data);

    ST7735S_DELAY();

    LCD_ST7735S_WR_L;
    LCD_ST7735S_WR_H;
    LCD_ST7735S_CS_H;
}

void LCD_ST7735S_wr_cmd(uint8_t cmd)
{
    LCD_ST7735S_RD_H;
    LCD_ST7735S_RS_L;
    LCD_ST7735S_CS_L;

    LCD_ST7735S_P0(cmd);

    ST7735S_DELAY();
    LCD_ST7735S_WR_L;
    LCD_ST7735S_WR_H;
    LCD_ST7735S_CS_H;
}
#endif

void LCD_ST7735S_GPIO_Init(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启GPIOB和GPIOF的外设时钟*/
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE); 

		/*选择要控制的GPIOB引脚*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	

		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*调用库函数，初始化GPIOB0*/
		GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
			/*选择要控制的GPIOB引脚*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;	

		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*调用库函数，初始化GPIOB0*/
		GPIO_Init(GPIOA, &GPIO_InitStructure);	
		
				/*选择要控制的GPIOB引脚*/															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	

		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*调用库函数，初始化GPIOB0*/
		GPIO_Init(GPIOC, &GPIO_InitStructure);


}

/*!
 *  @brief      LCD_ST7735S初始化
 *  @since      v5.0
 */
void    LCD_ST7735S_init()
{

		LCD_ST7735S_GPIO_Init();	
		
    ST7735S_DELAY();
    LCD_ST7735S_RST_H;

    //初始化总线
    LCD_ST7735S_RST_H;
    ST7735S_DELAY();
    LCD_ST7735S_RST_L;
    ST7735S_DELAY();
    LCD_ST7735S_RST_H;
    ST7735S_DELAY_MS(500);      //上电给足够时间

    LCD_ST7735S_WR_8CMD(0x11);           //Sleep out   退出睡眠模式
    ST7735S_DELAY_MS(120);      //Delay 120ms
    //------------------------------------ST7735S Frame Rate-----------------------------------------//
    /* Set the frame frequency of the full colors normal mode. */
    LCD_ST7735S_WR_8CMD(0xB1);     //In Normal Mode (Full Colors)    全屏模式
    LCD_ST7735S_WR_8DATA(0x05);    //设置 RTNA Set 1-line  Period  一行周期
    LCD_ST7735S_WR_8DATA(0x3A);    //设置 FPA: Front Porch
    LCD_ST7735S_WR_8DATA(0x3A);    //设置 BPA: Back Porch
    //Frame rate=fosc/((RTNA x 2 + 40) x (LINE + FPA + BPA +2))
    //其中 fosc = 850kHz

    /* Set the frame frequency of the Idle mode.  */
    LCD_ST7735S_WR_8CMD(0xB2);     //In Idle Mode (8-colors)
    LCD_ST7735S_WR_8DATA(0x05);
    LCD_ST7735S_WR_8DATA(0x3A);
    LCD_ST7735S_WR_8DATA(0x3A);

    /* Set the frame frequency of the Partial mode/ full colors. */
    LCD_ST7735S_WR_8CMD(0xB3);
    LCD_ST7735S_WR_8DATA(0x05);
    LCD_ST7735S_WR_8DATA(0x3A);
    LCD_ST7735S_WR_8DATA(0x3A);
    LCD_ST7735S_WR_8DATA(0x05);
    LCD_ST7735S_WR_8DATA(0x3A);
    LCD_ST7735S_WR_8DATA(0x3A);

    //------------------------------------End ST7735S Frame Rate-----------------------------------------//
    LCD_ST7735S_WR_8CMD(0xB4);   //Display Inversion Control  显示反转控制

    LCD_ST7735S_WR_8DATA(0x07);  //LCD_ST7735S_WR_8DATA(0x03); 原来为3，改为7
    // 低三位从高到低，分别为  full colors normal mode 、Idle mode 、
    // full Colors partial mode  的 点反转 或 列反转 ，1为列反转

    LCD_ST7735S_WR_8CMD(0xC0);   //Power Control Setting  电源控制设置
    LCD_ST7735S_WR_8DATA(0x28);
    LCD_ST7735S_WR_8DATA(0x08);
    LCD_ST7735S_WR_8DATA(0x84);
    LCD_ST7735S_WR_8CMD(0xC1);
    LCD_ST7735S_WR_8DATA(0XC0);
    LCD_ST7735S_WR_8CMD(0xC2);
    LCD_ST7735S_WR_8DATA(0x0C);
    LCD_ST7735S_WR_8DATA(0x00);
    LCD_ST7735S_WR_8CMD(0xC3);
    LCD_ST7735S_WR_8DATA(0x8C);
    LCD_ST7735S_WR_8DATA(0x2A);
    LCD_ST7735S_WR_8CMD(0xC4);
    LCD_ST7735S_WR_8DATA(0x8A);
    LCD_ST7735S_WR_8DATA(0xEE);
    //---------------------------------End ST7735S Power Sequence-------------------------------------//
    LCD_ST7735S_WR_8CMD(0xC5);   //  VCOM 电压配置
    LCD_ST7735S_WR_8DATA(0x0C); //  -0.725
    //------------------------------------ST7735S Gamma Sequence-----------------------------------------//
    LCD_ST7735S_WR_8CMD(0xE0);
    LCD_ST7735S_WR_8DATA(0x05);
    LCD_ST7735S_WR_8DATA(0x1A);
    LCD_ST7735S_WR_8DATA(0x0C);
    LCD_ST7735S_WR_8DATA(0x0E);
    LCD_ST7735S_WR_8DATA(0x3A);
    LCD_ST7735S_WR_8DATA(0x34);
    LCD_ST7735S_WR_8DATA(0x2D);
    LCD_ST7735S_WR_8DATA(0x2F);
    LCD_ST7735S_WR_8DATA(0x2D);
    LCD_ST7735S_WR_8DATA(0x2A);
    LCD_ST7735S_WR_8DATA(0x2F);
    LCD_ST7735S_WR_8DATA(0x3C);
    LCD_ST7735S_WR_8DATA(0x00);
    LCD_ST7735S_WR_8DATA(0x01);
    LCD_ST7735S_WR_8DATA(0x02);
    LCD_ST7735S_WR_8DATA(0x10);
    LCD_ST7735S_WR_8CMD(0xE1);
    LCD_ST7735S_WR_8DATA(0x04);
    LCD_ST7735S_WR_8DATA(0x1B);
    LCD_ST7735S_WR_8DATA(0x0D);
    LCD_ST7735S_WR_8DATA(0x0E);
    LCD_ST7735S_WR_8DATA(0x2D);
    LCD_ST7735S_WR_8DATA(0x29);
    LCD_ST7735S_WR_8DATA(0x24);
    LCD_ST7735S_WR_8DATA(0x29);
    LCD_ST7735S_WR_8DATA(0x28);
    LCD_ST7735S_WR_8DATA(0x26);
    LCD_ST7735S_WR_8DATA(0x31);
    LCD_ST7735S_WR_8DATA(0x3B);
    LCD_ST7735S_WR_8DATA(0x00);
    LCD_ST7735S_WR_8DATA(0x00);
    LCD_ST7735S_WR_8DATA(0x03);
    LCD_ST7735S_WR_8DATA(0x12);
    //------------------------------------End ST7735S Gamma Sequence-----------------------------------------//

    LCD_ST7735S_WR_8CMD(0x3A); //65k mode
    LCD_ST7735S_WR_8DATA(0x05);
    LCD_ST7735S_WR_8CMD(0x29); //开显示 Display on
    LCD_ST7735S_WR_8CMD(0x2c); //

    LCD_ST7735S_dir(st7735s_dir);

}

/*!
 *  @brief      设置ST7735S GRAM指针扫描方向
 *  @param      option    方向选择（正看（焊接排线在下面） 为 0 ，90度为1，180度为2，270度为2）
 *  @since      v5.0
 */
void LCD_ST7735S_dir(uint8_t option)
{
//    ASSERT(option < 4);

    st7735s_dir = option;
    switch(option)
    {
    case 0:
        LCD_ST7735S_WR_8CMD(0x36); // 内存数据访问控制  MX 列地址顺序, MY 行地址顺序 ，MV 行列交换 ，ML 垂直刷新顺序 ,RGB  RGB-BGA顺序
        LCD_ST7735S_WR_8DATA(0xc8);//0xc8  0xA8 0x08 0x68

        LCD_ST7735S_WR_8CMD(0x2a);
        LCD_ST7735S_WR_8DATA(0x00);
        LCD_ST7735S_WR_8DATA(0x02);
        LCD_ST7735S_WR_8DATA(0x00);
        LCD_ST7735S_WR_8DATA(0x81);

        LCD_ST7735S_WR_8CMD(0x2B);
        LCD_ST7735S_WR_8DATA(0x00);
        LCD_ST7735S_WR_8DATA(0x03);
        LCD_ST7735S_WR_8DATA(0x00);
        LCD_ST7735S_WR_8DATA(0x82);

        st7735s_h   = ST7735S_H;
        st7735s_w   = ST7735S_W;
        break;
    case 1:
        LCD_ST7735S_WR_8CMD(0x36); // 内存数据访问控制  MX 列地址顺序, MY 行地址顺序 ，MV 行列交换 ，ML 垂直刷新顺序 ,RGB  RGB-BGA顺序
        LCD_ST7735S_WR_8DATA(0xA8);//0xc8  0xA8 0x08 0x68

        LCD_ST7735S_WR_8CMD(0x2a);
        LCD_ST7735S_WR_8DATA(0x00);
        LCD_ST7735S_WR_8DATA(0x03);
        LCD_ST7735S_WR_8DATA(0x00);
        LCD_ST7735S_WR_8DATA(0x82);

        LCD_ST7735S_WR_8CMD(0x2B);
        LCD_ST7735S_WR_8DATA(0x00);
        LCD_ST7735S_WR_8DATA(0x02);
        LCD_ST7735S_WR_8DATA(0x00);
        LCD_ST7735S_WR_8DATA(0x81);

        st7735s_h   = ST7735S_W;
        st7735s_w   = ST7735S_H;
        break;
    case 2:
        LCD_ST7735S_WR_8CMD(0x36); // 内存数据访问控制  MX 列地址顺序, MY 行地址顺序 ，MV 行列交换 ，ML 垂直刷新顺序 ,RGB  RGB-BGA顺序
        LCD_ST7735S_WR_8DATA(0x08);//0xc8  0xA8 0x08 0x68

        LCD_ST7735S_WR_8CMD(0x2a);
        LCD_ST7735S_WR_8DATA(0x00);
        LCD_ST7735S_WR_8DATA(0x02);
        LCD_ST7735S_WR_8DATA(0x00);
        LCD_ST7735S_WR_8DATA(0x81);

        LCD_ST7735S_WR_8CMD(0x2B);
        LCD_ST7735S_WR_8DATA(0x00);
        LCD_ST7735S_WR_8DATA(0x03);
        LCD_ST7735S_WR_8DATA(0x00);
        LCD_ST7735S_WR_8DATA(0x82);

        st7735s_h   = ST7735S_H;
        st7735s_w   = ST7735S_W;
        break;
    case 3:
        LCD_ST7735S_WR_8CMD(0x36); // 内存数据访问控制  MX 列地址顺序, MY 行地址顺序 ，MV 行列交换 ，ML 垂直刷新顺序 ,RGB  RGB-BGA顺序
        LCD_ST7735S_WR_8DATA(0x68);//0xc8  0xA8 0x08 0x68

        LCD_ST7735S_WR_8CMD(0x2a);
        LCD_ST7735S_WR_8DATA(0x00);
        LCD_ST7735S_WR_8DATA(0x03);
        LCD_ST7735S_WR_8DATA(0x00);
        LCD_ST7735S_WR_8DATA(0x82);

        LCD_ST7735S_WR_8CMD(0x2B);
        LCD_ST7735S_WR_8DATA(0x00);
        LCD_ST7735S_WR_8DATA(0x02);
        LCD_ST7735S_WR_8DATA(0x00);
        LCD_ST7735S_WR_8DATA(0x81);

        st7735s_h   = ST7735S_W;
        st7735s_w   = ST7735S_H;
        break;
    default:
        break;
    }

}

/*!
 *  @brief      设置 ST7735S 开窗
 *  @param      site        左上角坐标位置
 *  @param      size        开窗大小
 *  @since      v5.0
 */
void LCD_ST7735S_ptlon(Site_t site, Size_t size)
{
    if(st7735s_dir&0x01)
    {
        site.x += 3;     //液晶需要偏移一下，避免四周看不到的行
        site.y += 2;
    }
    else
    {
        site.x += 2;     //液晶需要偏移一下，避免四周看不到的行
        site.y += 3;
    }
    LCD_ST7735S_WR_8CMD(0x2a);   //Partial Mode On  局部模式
    LCD_ST7735S_WR_8DATA((uint8_t)(site.x >> 8)); //高8位 Sx
    LCD_ST7735S_WR_8DATA((uint8_t)site.x);   //低8位 Sx
    LCD_ST7735S_WR_8DATA((uint8_t)((site.x + size.W - 1) >> 8));
    LCD_ST7735S_WR_8DATA((uint8_t)(site.x + size.W - 1));

    LCD_ST7735S_WR_8CMD(0x2B);//Row Address Set  行地址设置
    LCD_ST7735S_WR_8DATA((uint8_t)(site.y >> 8));
    LCD_ST7735S_WR_8DATA((uint8_t)site.y);
    LCD_ST7735S_WR_8DATA((uint8_t)((site.y + size.H - 1) >> 8));
    LCD_ST7735S_WR_8DATA((uint8_t)(site.y + size.H - 1));

}

/*!
 *  @brief      获取 ST7735S 高度
 *  @return     ILI9341 高度
 *  @since      v5.0
 */
uint16_t ST7735S_get_h()
{
    return st7735s_h;
}

/*!
 *  @brief      获取 ST7735S 宽度
 *  @return     ILI9341 宽度
 *  @since      v5.0
 */
uint16_t ST7735S_get_w()
{
    return st7735s_w;
}

/*!
 *  @brief      获取 ST7735S 显示方向
 *  @return     ST7735S 方向
 *  @since      v5.0
 */
uint8_t ST7735S_get_dir()
{
    return st7735s_dir;
}



