/*
*********************************************************************************************************
*
*	模块名称 : TFT液晶显示器驱动模块
*	文件名称 : bsp_ili9341_lcd.c
*	版    本 : V1.0
*	说    明 : 驱动芯片的访问地址为:  0x60000000
*				
*	修改记录 :
*		版本号    日期          作者                 说明
*		v1.0    2011-10-31   WildFire Team  ST固件库版本 V3.5.0版本。
*
*	Copyright (C), 2012-2013, WildFire Team
*
*********************************************************************************************************
*/
#include "bsp_touch.h"




#define	CHX 	0x90 	//通道Y+的选择控制字	
#define	CHY 	0xd0	//通道X+的选择控制字

#define	 SPI_CLK	  GPIO_Pin_7
#define  SPI_CS		  GPIO_Pin_10
#define	 SPI_MOSI	  GPIO_Pin_11
#define	 SPI_MISO	  GPIO_Pin_6
#define  TP_INT_PIN	  GPIO_Pin_9

#define TP_DCLK(a)	\
						if (a)	\
						GPIO_SetBits(GPIOG,GPIO_Pin_7);	\
						else		\
						GPIO_ResetBits(GPIOG,GPIO_Pin_7)
#define TP_CS(a)	\
						if (a)	\
						GPIO_SetBits(GPIOF,GPIO_Pin_10);	\
						else		\
						GPIO_ResetBits(GPIOF,GPIO_Pin_10)
#define TP_DIN(a)	\
						if (a)	\
						GPIO_SetBits(GPIOF,GPIO_Pin_11);	\
						else		\
						GPIO_ResetBits(GPIOF,GPIO_Pin_11)

#define TP_DOUT		 GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_6)//	//数据输出

/*
*********************************************************************************************************
*	函 数 名: bsp_InitTouch
*	功能说明: XPT2046触摸芯片初始化
*             PB7   --> TP_CS
*			  PA5   --> TP_SCK
*			  PA6   --> TP_MISO
*			  PA7   --> TP_MOSI
*			  PB6   --> TP_PEN_INT
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitTouch(void) 
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* 开启GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG | RCC_APB2Periph_GPIOF, ENABLE);

    //////下面是SPI相关GPIO初始化//////            
    GPIO_InitStructure.GPIO_Pin=SPI_CLK;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz ;	  
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_Init(GPIOG, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = SPI_MOSI;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin = SPI_MISO; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      
    GPIO_Init(GPIOF, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = SPI_CS; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      
    GPIO_Init(GPIOF, &GPIO_InitStructure);		

    GPIO_ResetBits(GPIOF,SPI_CS);
}

/*
*********************************************************************************************************
*	函 数 名: XPT2046_WriteCMD
*	功能说明: 写命令
*	形    参：CHX 	0x90 	//通道Y+的选择控制字 CHY 	0xd0	//通道X+的选择控制字
*	返 回 值: 无
*********************************************************************************************************
*/
static void XPT2046_WriteCMD(unsigned char cmd) 
{
    unsigned char buf;
    unsigned char i;
//     TP_CS(1);
    TP_DIN(0);
    TP_DCLK(0);
//     TP_CS(0);
    for(i=0;i<8;i++) 
    {
        buf=(cmd>>(7-i))&0x1;
        TP_DIN(buf);
        //Delayus(5);
        TP_DCLK(1);
        //Delayus(5);
        TP_DCLK(0);
    }
}

/*
*********************************************************************************************************
*	函 数 名: XPT2046_ReadCMD
*	功能说明: 选择一个模拟通道，启动ADC，并返回ADC采样结果
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/

static unsigned short XPT2046_ReadCMD(void) 
{
    unsigned short buf=0,temp;
    unsigned char i;
    TP_DIN(0);
    TP_DCLK(1);
    for(i=0;i<12;i++) 
    {
        TP_DCLK(0);          
        temp= (TP_DOUT) ? 1:0;
        buf|=(temp<<(11-i));
        TP_DCLK(1);
    }
    buf&=0x0fff;

    return(buf);
}

/*
*********************************************************************************************************
*	函 数 名: TSC2046_ReadAdc
*	功能说明: 选择一个模拟通道，启动ADC，并返回ADC采样结果
*	形    参：_ucCh = 0x90 表示Y通道； 0xd0 表示X通道
*	返 回 值: 12位ADC值
*********************************************************************************************************
*/
uint16_t XPT2046_ReadAdc(uint8_t _ucCh)
{
	uint16_t usAdc;

    XPT2046_WriteCMD(_ucCh);

	return 	XPT2046_ReadCMD();
}
