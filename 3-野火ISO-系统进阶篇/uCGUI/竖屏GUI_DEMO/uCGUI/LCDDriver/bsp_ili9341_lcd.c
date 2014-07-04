/*
*********************************************************************************************************
*
*	模块名称 : TFT液晶显示器驱动模块
*	文件名称 : bsp_ili9341_lcd.c
*	版    本 : V1.1
*	说    明 : 驱动芯片的访问地址为:  0x60000000
*				
*	修改记录 :
*		版本号    日期          作者                 说明
*		v1.0    2011-10-31   WildFire Team  ST固件库版本 V3.5.0版本。
*       v1.1    2011-11-07   WildFire Team  修正竖屏显示，通过直接将X,Y都设置为递减，加速GUI显示。
*
*	Copyright (C), 2012-2013, WildFire Team
*
*********************************************************************************************************
*/
#include "bsp_ili9341_lcd.h"


/* ILI9341寄存器地址 */
#define SWRESET 	0x01 //Software Reset
#define RDDIDIF 	0x04 //Read Display Identification Information
#define RDDST   	0x09 //Read Display Status
#define RDDPM   	0x0A //Read Display Power Mode
#define RDDMADCTL	0x0B //Read Display MADCTL
#define RDDCOLMOD	0x0C //Read Display Pixel Format
#define RDDIM		0x0D //Read Display Image Mode
#define RDDSM		0x0E //Read Display Signal Mode
#define RDDSDR		0x0F //Read Display Self-Diagnostic Result
#define SPLIN		0x10 //Enter Sleep Mode
#define SLPOUT		0x11 //Sleep Out
#define PTLON		0x12 //Partial Mode On
#define NORON		0x13 //Normal Display Mode On
#define DINVOFF		0x20 //Display Inversion OFF
#define DINVON		0x21 //Display Inversion ON
#define GAMSET      0x26 //Gamma Set
#define DISPOFF		0x28 //Display OFF
#define DISPON		0x29 //Display ON
#define CASET		0x2A //Column Address Set
#define PASET		0x2B //Page Address Set
#define RAMWR		0x2C //Memory Write
#define RGBSET		0x2D //Color Set
#define RAMRD       0x2E //Memory Read
#define PLTAR		0x30 //Partial Area
#define VSCRDEF		0x33 //Vertical Scrolling Definition
#define TEOFF		0x34 //Tearing Effect Line OFF
#define TEON		0x35 //Tearing Effect Line ON
#define MADCTL		0x36 //Memory Access Control
#define VSCRSADD	0x37 //Vertical Scrolling Start Address
#define IDMOFF		0x38 //Idle Mode OFF
#define IDMON		0x39 //Idle Mode ON
#define PIXSET		0x3A //Pixel Format Set
#define RAMWRC      0x3C //Write Memory Continue
#define RAMRDC		0x3E //Read  Memory Continue
#define STTS		0x44 //Set Tear Scanline 
#define GTSL		0x45 //Get Scan line
#define WRDISBV		0x51 //Write Display Brightness
#define RDDISBV		0x52 //Read Display Brightness Value
#define WRCTRLD		0x53 //Write Control Display
#define RDCTRLD		0x54 //Read Control Display
#define WRCABC		0x55 //Write Content Adaptive Brightness Control
#define RDCABC		0x56 //Read Content Adaptive Brightness Control
#define WRCABCMIN	0x5E //Write CABC Minimum Brightness
#define RDCABCMIN	0x5F //Read CABC Minimum Brightnes
#define RDID1		0xDA //Read ID1
#define RDID2		0xDB //Read ID2
#define RDID3		0xDC //Read ID3
#define IFMODE		0xB0 //Interface Mode Control
#define FRMCTR1		0xB1 //Frame Rate Control (In Normal Mode / Full colors
#define FRMCTR2		0xB2 //Frame Rate Control (In Idle Mode / 8l colors)
#define FRMCTR3		0xB3 //Frame Rate Control (In Partial Mode / Full colors)
#define INVTR		0xB4 //Display Inversion Control
#define PRCTR		0xB5 //Blanking Porch
#define DISCTRL		0xB6 //Display Function Control
#define ETMOD		0xB7 //Entry Mode Set
#define BKCTL1		0xB8 //Backlight Control 1 
#define BKCTL2		0xB9 //Backlight Control 2 
#define BKCTL3		0xBA //Backlight Control 3 
#define BKCTL4		0xBB //Backlight Control 4 
#define BKCTL5		0xBC //Backlight Control 5
#define BKCTL7		0xBE //Backlight Control 7
#define BKCTL8		0xBF //Backlight Control 8
#define PWCTRL1		0xC0 //Power Control 1
#define PWCTRL2		0xC1 //Power Control 2
#define VMCTRL1		0xC5 //VCOM Control 1
#define VMCTRL2		0xC7 //VCOM Control 2
#define NVMWR		0xD0 //NV Memory Write
#define NVMPKEY		0xD1 //NV Memory Protection Key
#define RDNVM		0xD2 //NV Memory Status Read
#define RDID4		0xD3 //Read ID4
#define PGAMCTRL	0xE0 //Positive Gamma Control
#define NGAMCTRL	0xE1 //Negative Gamma Correction
#define DGAMCTRL1	0xE2 //Digital Gamma Control 1
#define DGAMCTRL2	0xE3 //Digital Gamma Control 2
#define IFCTL		0xF6 //16bits Data Format Selection
#define PWCTLA		0xCB //Power control A 
#define PWCTLB		0xCF //Power control B 
#define DTIMCTLA	0xE8 //Driver timing control A 
#define DTIMCTLB	0xEA //Driver timing control B 
#define PWONSCTL	0xED //Power on sequence control 
#define EN3G		0xF2 //Enable_3G 
#define PRCTL		0xF7 //Pump ratio control 


/* 定义 */
#define LCD_ILI9341_CMD(index)       LCD_WR_REG(index)
#define LCD_ILI9341_Parameter(val)   LCD_WR_Data(val)

/* 定义LCD驱动器用到的延时 */
#define DEBUG_DELAY()   Delay(2000)


/*
*********************************************************************************************************
*	函 数 名: LCD_WR_REG
*	功能说明: 写寄存器地址函数
*	形    参: index 寄存器地址
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_WR_REG(unsigned int index)
{
	ILI9341_REG = index;

}

/*
*********************************************************************************************************
*	函 数 名: LCD_WR_CMD
*	功能说明: 写寄存器数据函数
*	形    参: index 寄存器地址
*			  val   数据
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_WR_CMD(unsigned int index,unsigned int val)
{	
	ILI9341_REG = index;	
	ILI9341_RAM = val;
}

/*
*********************************************************************************************************
*	函 数 名: LCD_WR_CMD
*	功能说明: 读GRAM数据-
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t LCD_RD_data(void)	
{	
	uint16_t R=0, G=0, B=0 ;

	R = ILI9341_RAM; 	/*FIRST READ OUT DUMMY DATA*/
	R = ILI9341_RAM;  	/*READ OUT RED DATA  */
	B = ILI9341_RAM;  	/*READ OUT BLACK DATA*/
	G = ILI9341_RAM;  	/*READ OUT GREEN DATA*/
	
    return (((R>>11)<<11) | ((G>>10)<<5) | (B>>11)) ;
}

/*
*********************************************************************************************************
*	函 数 名: LCD_WR_Data
*	功能说明: 写16位数据函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_WR_Data(unsigned int val)
{   
	ILI9341_RAM = val; 	
}

/*
*********************************************************************************************************
*	函 数 名: Delay
*	功能说明: 延时
*	形    参: nCount 延时计数
*	返 回 值: 无
*********************************************************************************************************
*/
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

/*
*********************************************************************************************************
*	函 数 名: LCD_FSMCConfig
*	功能说明: FSMC初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_FSMCConfig(void)
{	
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  p;
	
	/*-- FSMC Configuration ------------------------------------------------------*/
	/*----------------------- SRAM Bank 1 ----------------------------------------*/
	/* FSMC_Bank1_NORSRAM4 configuration */
	p.FSMC_AddressSetupTime = 2;
	p.FSMC_AddressHoldTime = 0;
	
	/* LCD 读需要设置为4，只写的话设置1即可 */
	p.FSMC_DataSetupTime = 5;
	
	p.FSMC_BusTurnAroundDuration = 0;
	p.FSMC_CLKDivision = 0;
	p.FSMC_DataLatency = 0;
	p.FSMC_AccessMode = FSMC_AccessMode_B;
	
	/* Color LCD configuration ------------------------------------
	 LCD configured as follow:
	    - Data/Address MUX = Disable
	    - Memory Type = SRAM
	    - Data Width = 16bit
	    - Write Operation = Enable
	    - Extended Mode = Enable
	    - Asynchronous Wait = Disable */
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
	
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);
	
	/* BANK 1 (of NOR/SRAM Bank 1~4) is enabled */
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}

/*
*********************************************************************************************************
*	函 数 名: LCD_CtrlLinesConfig
*	功能说明: TFT的GPIO初始化
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_CtrlLinesConfig(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
    
    /* 使能FSMC时钟*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
    
    /* 使能FSMC对应相应管脚时钟*/
	    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | 
	                            RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOG | 
	                            RCC_APB2Periph_GPIOF , ENABLE);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    /* 配置LCD背光控制管脚*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;		
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    /* 配置LCD复位控制管脚*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 	 
    GPIO_Init(GPIOG, &GPIO_InitStructure);  		   
    
    /* 配置FSMC相对应的数据线,FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10*/	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | 
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
    /* 配置FSMC相对应的控制线
	 * PD4-FSMC_NOE  :LCD-RD
   * PD5-FSMC_NWE  :LCD-WR
	 * PD7-FSMC_NE1  :LCD-CS
   * PD11-FSMC_A16 :LCD-DC
	 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
    GPIO_Init(GPIOG, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ; 
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
	/* 打开背光 */
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);
}

/*
*********************************************************************************************************
*	函 数 名: LCD_Reset
*	功能说明: 复位
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD_Reset(void)
{			
	/*复位 LCD*/	 
    GPIO_ResetBits(GPIOG, GPIO_Pin_11);
    Delay(0xAFFf<<2);					   
    GPIO_SetBits(GPIOG, GPIO_Pin_11 );	
	Delay(0xAFFf<<2);	
}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitLCD
*	功能说明: ili9341初始化
*	形    参: 无
*	返 回 值: 无
*   	36H 寄存器(MV用于X,Y切换)
*       MY      Row Address Order 
		MX		Column Address Order
		MV      Row / Column Exchange 
		7 	6		  
        MY  MX  
        0   0     X递增，Y递增    
 		0   1     X递增，Y递减    	
        1   0     X递减，Y递增  
        1   1     X递减，Y递减 

		下面以野火的板子为例说明，界面采用手机那样的形式，所以取X,Y都是递减，3个位可以实现
		8中排序，这里不考虑X，Y的交换。（野火LCD屏上的字体“野火”正的时候，作为正面，右下角是
		起始点，左上角是结束点）
		本例程采用方式四  36H = 0xCx

		    36H = 0x0x                    36H = 0x4x	  		                    
		  -------------------          -------------------
		 |(239,319)          |        |(239,319)          |
		 |                   |        |
		 |  ^           ^    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |  |  <------  |    |        |   | <------   |   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |                   |        |	  v			  v	  |
		 |                   |        |                   |
		 |              (0,0)|        |              (0,0)|
		 |-------------------|        |-------------------|
		 |                   |        |                   |
		  -------------------          -------------------
			36H = 0x8x					   36H = 0xCx 
		  -------------------          -------------------
		 |(239,319)          |        |(239,319)          |
		 |                   |        |
		 |  ^           ^    |        |                   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |  |  <------  |    |        |   | ------>   |   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |  |           |    |        |   |           |   |
		 |                   |        |	  v			  v	  |
		 |                   |        |                   |
		 |              (0,0)|        |              (0,0)|
		 |-------------------|        |-------------------|
		 |                   |        |                   |
		  -------------------          -------------------
*********************************************************************************************************
*/
void bsp_InitLCD(void)
{
	/* 配置LCD控制口线GPIO */
	LCD_CtrlLinesConfig();

	/* 配置FSMC接口，数据总线 */
	LCD_FSMCConfig();

	/* 复位 */
	LCD_Reset();
		
	/*  Power control B (CFh)  */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xCF);
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0x81);
	LCD_ILI9341_Parameter(0x30);
	
	/*  Power on sequence control (EDh) */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xED);
	LCD_ILI9341_Parameter(0x64);
	LCD_ILI9341_Parameter(0x03);
	LCD_ILI9341_Parameter(0x12);
	LCD_ILI9341_Parameter(0x81);
	
	/*  Driver timing control A (E8h) */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xE8);
	LCD_ILI9341_Parameter(0x85);
	LCD_ILI9341_Parameter(0x10);
	LCD_ILI9341_Parameter(0x78);
	
	/*  Power control A (CBh) */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xCB);
	LCD_ILI9341_Parameter(0x39);
	LCD_ILI9341_Parameter(0x2C);
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0x34);
	LCD_ILI9341_Parameter(0x02);
	
	/* Pump ratio control (F7h) */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xF7);
	LCD_ILI9341_Parameter(0x20);
	
	/* Driver timing control B */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xEA);
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0x00);
	
	/* Frame Rate Control (In Normal Mode/Full Colors) (B1h) */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xB1);
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0x1B);
	
	/*  Display Function Control (B6h) */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xB6);
	LCD_ILI9341_Parameter(0x0A);
	LCD_ILI9341_Parameter(0xA2);
	
	/* Power Control 1 (C0h) */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xC0);
	LCD_ILI9341_Parameter(0x35);
	
	/* Power Control 2 (C1h) */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xC1);
	LCD_ILI9341_Parameter(0x11);
	
	/* VCOM Control 1(C5h) */
	LCD_ILI9341_CMD(0xC5);
	LCD_ILI9341_Parameter(0x45);
	LCD_ILI9341_Parameter(0x45);
	
	/*  VCOM Control 2(C7h)  */
	LCD_ILI9341_CMD(0xC7);
	LCD_ILI9341_Parameter(0xA2);
	
	/* Enable 3G (F2h) */
	LCD_ILI9341_CMD(0xF2);
	LCD_ILI9341_Parameter(0x00);
	
	/* Gamma Set (26h) */
	LCD_ILI9341_CMD(0x26);
	LCD_ILI9341_Parameter(0x01);

	/* Positive Gamma Correction */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0xE0); 
	LCD_ILI9341_Parameter(0x0F);
	LCD_ILI9341_Parameter(0x26);
	LCD_ILI9341_Parameter(0x24);
	LCD_ILI9341_Parameter(0x0B);
	LCD_ILI9341_Parameter(0x0E);
	LCD_ILI9341_Parameter(0x09);
	LCD_ILI9341_Parameter(0x54);
	LCD_ILI9341_Parameter(0xA8);
	LCD_ILI9341_Parameter(0x46);
	LCD_ILI9341_Parameter(0x0C);
	LCD_ILI9341_Parameter(0x17);
	LCD_ILI9341_Parameter(0x09);
	LCD_ILI9341_Parameter(0x0F);
	LCD_ILI9341_Parameter(0x07);
	LCD_ILI9341_Parameter(0x00);

	/* Negative Gamma Correction (E1h) */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0XE1); 
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0x19);
	LCD_ILI9341_Parameter(0x1B);
	LCD_ILI9341_Parameter(0x04);
	LCD_ILI9341_Parameter(0x10);
	LCD_ILI9341_Parameter(0x07);
	LCD_ILI9341_Parameter(0x2A);
	LCD_ILI9341_Parameter(0x47);
	LCD_ILI9341_Parameter(0x39);
	LCD_ILI9341_Parameter(0x03);
	LCD_ILI9341_Parameter(0x06);
	LCD_ILI9341_Parameter(0x06);
	LCD_ILI9341_Parameter(0x30);
	LCD_ILI9341_Parameter(0x38);
	LCD_ILI9341_Parameter(0x0F);
	

	/*设置成竖屏的显示方式*/ 
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0x36); 
	LCD_ILI9341_Parameter(0xc8);    
	DEBUG_DELAY();

	/* 设置X轴的起始和结束坐标 */
	LCD_ILI9341_CMD(0X2A); 
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0xEF);
	
	/* 设置Y轴的起始和结束坐标 */	
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0X2B); 
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0x00);
	LCD_ILI9341_Parameter(0x01);
	LCD_ILI9341_Parameter(0x3F);

	/*  Pixel Format Set (3Ah)  */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0x3a); 
	LCD_ILI9341_Parameter(0x55);

	/* Sleep Out (11h)  */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0x11); 

	/* Display ON (29h) */
	DEBUG_DELAY();
	LCD_ILI9341_CMD(0x29); 
    						
}

/*
*********************************************************************************************************
*	函 数 名: Set_direction
*	功能说明: 设置屏幕座标
*	形    参: _usX1      行座标
*             _usY1      列座标
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD9341_SetCursor(uint16_t _usX1 , uint16_t _usY1)	
{				
		
	/*竖屏*/
	LCD_ILI9341_CMD(0X2A); 				 /* 设置X坐标 */
	LCD_ILI9341_Parameter(_usX1>>8);	 /* 先高8位，然后低8位 */
	LCD_ILI9341_Parameter(_usX1&0xff);	 /* 设置起始点和结束点*/
	LCD_ILI9341_Parameter(_usX1>>8);
	LCD_ILI9341_Parameter(_usX1&0xff);

    LCD_ILI9341_CMD(0X2B); 			     /* 设置Y坐标*/
	LCD_ILI9341_Parameter(_usY1>>8);
	LCD_ILI9341_Parameter(_usY1&0xff);
	LCD_ILI9341_Parameter(_usY1>>8);
	LCD_ILI9341_Parameter(_usY1&0xff);  
		     
}

/*
*********************************************************************************************************
*	函 数 名: LCD_Clear
*	功能说明: 将屏幕填充成指定的颜色，如清屏，则填充 0xffff
*	形    参: dat      填充值
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD9341_Clear(uint16_t _usColor)	
{
    uint32_t n;

	/***************竖屏************/
	LCD_ILI9341_CMD(0X2A); 			  /* 设置X坐标 */
	LCD_ILI9341_Parameter(0);		  /* 先高8位，然后低8位 */
	LCD_ILI9341_Parameter(0);		  /* 设置起始点和结束点*/
	LCD_ILI9341_Parameter(239>>8);
	LCD_ILI9341_Parameter(239&0xff);
	
	LCD_ILI9341_CMD(0X2B); 			 /* 设置Y坐标*/
	LCD_ILI9341_Parameter(0);
	LCD_ILI9341_Parameter(0);
	LCD_ILI9341_Parameter(319>>8);
	LCD_ILI9341_Parameter(319&0xff);
		
	LCD_ILI9341_CMD(0x2c);
	for(n=0; n<320*240; n++)
	{
		LCD_WR_Data(_usColor);
	} 
}

/*
*********************************************************************************************************
*	函 数 名: LCD9341_GetPoint
*	功能说明: 获取指定座标的颜色值
*	形    参: _usX1      行座标
*             _usY1      列座标
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t LCD9341_GetPoint(uint16_t _usX1 , uint16_t _usY1)
{ 
    uint16_t temp;

	LCD9341_SetCursor(_usX1,_usY1);
    LCD_ILI9341_CMD(0x2e);         /* 读数据 */
	temp=LCD_RD_data();
    return (temp);
}

/*
*********************************************************************************************************
*	函 数 名: LCD9341_SetPoint
*	功能说明: 在指定座标画点
*	形    参: _usX1      行座标
*             _usY1      列座标
*             _usColor   点的颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD9341_SetPoint(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usColor)	
{   	
	
	LCD9341_SetCursor(_usX1,_usY1);
	LCD_ILI9341_CMD(0x2c);	         /* 写数据 */
	LCD_WR_Data(_usColor);

}

/*
*********************************************************************************************************
*	函 数 名: LCD_SetDispWin
*	功能说明: 设置窗口区域
*	形    参: _usX1    窗户起点
           	  _usY1	   窗户起点
              _usX2    窗户结束点 
              _usY2    窗户结束点
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD9341_SetDispWin(uint16_t _usX1, uint16_t _usY1, uint16_t _usX2, uint16_t _usY2)
{                    

	LCD_ILI9341_CMD(0X2A); 				  /* 设置X坐标 */
	LCD_ILI9341_Parameter(_usX1>>8);	  /* 设置高8位和低8位*/
	LCD_ILI9341_Parameter(_usX1&0xff);	  /* 设置起始点和结束点*/
	LCD_ILI9341_Parameter(_usX2>>8);	
	LCD_ILI9341_Parameter(_usX2&0xff);
	
	LCD_ILI9341_CMD(0X2B); 				  /* 设置Y坐标*/
	LCD_ILI9341_Parameter(_usY1>>8);   
	LCD_ILI9341_Parameter(_usY1&0xff);
	LCD_ILI9341_Parameter(_usY2>>8);   
	LCD_ILI9341_Parameter(_usY2&0xff);

	LCD_ILI9341_CMD(0x2c);     
}

/*
*********************************************************************************************************
*	函 数 名: LCD9341_DrawHLine
*	功能说明: 画水平线 用UCGUI的接口函数
*	形    参: X,Y的坐标和颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD9341_DrawHLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usX2 , uint16_t _usColor)
{
	uint16_t i;

#if 1
	
	LCD9341_SetDispWin(_usX1, _usY1, _usX2,_usY1);

	for (i = 0; i <_usX2-_usX1+1; i++)
	{
		ILI9341_RAM = _usColor;
	}

#else

	for (i = _usX1; i <=_usX2; i++)
	{	
		LCD9341_SetPoint(i, _usY1, _usColor);	
	}

#endif

}

/*
*********************************************************************************************************
*	函 数 名: LCD9341_DrawVLine
*	功能说明: 画垂直平线 用UCGUI的接口函数
*	形    参: X,Y的坐标和颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD9341_DrawVLine(uint16_t _usX1 , uint16_t _usY1 , uint16_t _usY2 , uint16_t _usColor)
{
	uint16_t i;
#if 1

	LCD9341_SetDispWin(_usX1, _usY1,_usX1,_usY2);

	for (i = 0; i <_usY2-_usY1+1; i++)
	{
		ILI9341_RAM = _usColor;
	}
#else

	for (i = _usY1; i <=_usY2; i++)
	{	
		LCD9341_SetPoint(_usX1, i, _usColor);	
	}

#endif
}

/*
*********************************************************************************************************
*	函 数 名: LCD9341_DrawVLine
*	功能说明: 画矩形填充 用UCGUI的接口函数
*	形    参: X,Y的坐标和颜色
*	返 回 值: 无
*********************************************************************************************************
*/
void LCD9341_FillRect(uint16_t _usX1 , uint16_t _usY1 ,  uint16_t _usX2 , uint16_t _usY2 , uint16_t _usColor)
{                    
   uint32_t n, temp;
   
   LCD9341_SetDispWin(_usX1, _usY1,_usX2,_usY2); 
    
   temp = (u32)(_usX2-_usX1+1)*(_usY2 -_usY1+1);
       
   for(n=0; n<temp; n++)
   {
		ILI9341_RAM =_usColor;
   }
	 	  
}
