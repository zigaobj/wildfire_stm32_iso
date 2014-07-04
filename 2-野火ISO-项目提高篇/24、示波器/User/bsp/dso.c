/*
*********************************************************************************************************
*	                                  
*	模块名称 : 示波器模块。
*	文件名称 : dso.c
*	版    本 : V2.0
*	说    明 : 示波器主程序。
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2011-08-27 armfly  ST固件库V3.5.0版本。
*		v2.0    2011-10-16 armfly  优化工程结构。
*
*	Copyright (C), 2010-2011, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include "bsp_tft_lcd.h"
#include "bsp_timer.h"
#include "bsp_button.h"
#include "dso.h"
#include "WaveMaker.h"

#define DSO_VER	"V0.7"		  /* 当前版本 */

/* 主程序状态字 */
enum
{
	S_HELP		= 0,	/* 帮助页面 */
	S_RUN		= 1,	/* 采集状态 */
	S_HOLD		= 2		/* 暂停状态 */
};

/* 时间分度表， g_DSO.TimeBaseId作为数组索引  */
const uint32_t g_TimeTable[] =
{
	20, 	50, 	100, 
	200,	500,	1000,
	2000,	5000,	10000,
	20000,	50000,	100000,
	200000,	500000,	1000000		
};

/* 衰减倍数表  
	g_DSO.Ch1Attenuation = 23
	幅度衰减因子:
	(1) 28 === 1V  (1:1)
*/
#define ATT_COUNT	6
const uint32_t g_AttTable[ATT_COUNT][2] =
{
	/* {除数,每大格电压}  1:1 */
	{1400,	5000},
	{560,	2000},
	{280,	1000},
	{140,	500},
	{56,	200},
	{28,	100},
};

static void DsoHelp(uint8_t *pMainStatus);
static void DsoRun(uint8_t *pMainStatus);

/*
	安富莱STM32F103ZE-EK 口线分配：

	示波器通道1同时连接到CPU的PC0、PC2、PC3，这3个口线作为ADC输入使用
		PC0 = ADC1_IN10
		PC2 = ADC2_IN12
		PC3 = ADC3_IN13

	示波器通道2只连接到CPU的PC1，这个口线作为ADC输入使用
		PC1 = ADC2_IN12

	对于通道1的模拟信号，可以同时启动3个ADC进行交错采样，理论上可以实现3Mbps采样速率 
	(后来发现ADC2不能DMA操作，因此这句话还有待考究)

	通道2只能启动1个ADC进行采样。
	支持通道1和通道2进行同步采样。
*/

DSO_T g_DSO;	/* 全局变量，是一个结构体 */

/*
*********************************************************************************************************
*	函 数 名: DsoMain
*	功能说明: DSO主程序，是一个死循环
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void DsoMain(void)
{
	uint8_t MainStatus = S_RUN;		/* 程序执行状态 */

	InitWaveMaker();	/* 初始化DAC，并在DAC1引脚产生频率10KHz，幅度2V的正弦波 */

	LCD_InitHard();		/* 显示器初始化 */

	LCD_SetBackLight(BRIGHT_DEFAULT);  /* 设置背光亮度 */

	/* 进入主程序循环体 */
	while (1)
	{
		switch (MainStatus)
		{
			case S_HELP:
				DsoHelp(&MainStatus);		/* 显示帮助 */
				break;

			case S_RUN:
				DsoRun(&MainStatus);		/* 全速采集，实时显示 */
				break;

			default:
				break;
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: InitDSO
*	功能说明: 对示波器通道1进行初始化配置。主要完成GPIO的配置、ADC的配置、DMA配置。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitDSO(void)
{
	/* 配置GPIO.  */
	{
		GPIO_InitTypeDef GPIO_InitStructure;

		/* 打开GPIO_C 和 AFIO 的时钟 */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC	| RCC_APB2Periph_AFIO, ENABLE);

		/* 配置PC0为模拟输入模式 */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}


	/* 配置DMA1_1 用于CH1 */
	#if 1
	{
		DMA_InitTypeDef DMA_InitStructure;

		/* 使能 DMA1 时钟 */
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

		DMA_DeInit(DMA1_Channel1);		/* 复位DMA1寄存器到缺省状态 */
		DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	/* 选择ADC1的数据寄存器作为源 */
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_DSO.Ch1Buf;	/* 目标地址 */
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	/* 设置DMA传输方向，外设(ADC)作为源 */
		DMA_InitStructure.DMA_BufferSize = SAMPLE_COUNT;	/* 设置缓冲区大小 */
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	/* 外设地址不自增 */
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	/* 存储器地址需要自增 */
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	/* 选择外设传输单位：16bit */
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	/* 选择内存传输单位：16bit */
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	/* 无需循环模式   */
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;	/* 选择DMA优先级 */
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	/* DMA传输类型，不是内存到内存 */
		DMA_Init(DMA1_Channel1, &DMA_InitStructure);

		/* 使能 DMA1 通道1 */
		DMA_Cmd(DMA1_Channel1, ENABLE);
	}
	#endif

	/* 配置DMA2_5 用于CH2 */
	#if 1
	{
		DMA_InitTypeDef DMA_InitStructure;

		/* 使能 DMA2 时钟 */
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

		DMA_DeInit(DMA2_Channel5);		/* 复位DMA2寄存器到缺省状态 */
		DMA_InitStructure.DMA_PeripheralBaseAddr = ADC3_DR_Address;	/* 选择ADC3的数据寄存器作为源 */
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_DSO.Ch2Buf;	/* 目标地址 */
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	/* 设置DMA传输方向，外设(ADC)作为源 */
		DMA_InitStructure.DMA_BufferSize = SAMPLE_COUNT;	/* 设置缓冲区大小 */
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	/* 外设地址不自增 */
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	/* 存储器地址需要自增 */
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	/* 选择外设传输单位：16bit */
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	/* 选择内存传输单位：16bit */
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	/* 无需循环模式   */
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;	/* 选择DMA优先级 */
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	/* DMA传输类型，不是内存到内存 */
		DMA_Init(DMA2_Channel5, &DMA_InitStructure);

		/* Enable DMA2 channel5 */
		DMA_Cmd(DMA2_Channel5, ENABLE);
	}
	#endif

	/* 配置ADC1  */
	
	 /* ADCCLK = PCLK2/2 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div2);

	#if 1
	{
		ADC_InitTypeDef	ADC_InitStructure;

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_InitStructure.ADC_ScanConvMode = DISABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		/* 连续转换静止 */
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC3;	/* 选择TIM1的CC3做触发 */

		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	/* 数据右对齐,高位为0 */
		ADC_InitStructure.ADC_NbrOfChannel = 1;	/* 1个通道 */
		ADC_Init(ADC1, &ADC_InitStructure);

		/* ADC1 规则通道配置 */
		ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_1Cycles5);

		/* 使能 ADC1 外部触发转换 */
		ADC_ExternalTrigConvCmd(ADC1, ENABLE);

		/* 使能 ADC1 */
		ADC_Cmd(ADC1, ENABLE);

		/* 使能 ADC1 DMA */
		ADC_DMACmd(ADC1, ENABLE);

		/* 使能 ADC1 复位校准寄存器 */
		ADC_ResetCalibration(ADC1);
		/* 检测复位校准寄存器 */
		while(ADC_GetResetCalibrationStatus(ADC1));

		/* 开始 ADC1 校准 */
		ADC_StartCalibration(ADC1);
		/* 等待校准结束 */
		while(ADC_GetCalibrationStatus(ADC1));
	}
	#endif

	/* 配置ADC3  */
	#if 1
	{
		ADC_InitTypeDef	ADC_InitStructure;

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_InitStructure.ADC_ScanConvMode = DISABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;		/* 连续转换静止 */
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC3;	/* 选择TIM1的CC3做触发 */
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	/* 数据右对齐,高位为0 */
		ADC_InitStructure.ADC_NbrOfChannel = 1;	/* 1个通道 */
		ADC_Init(ADC3, &ADC_InitStructure);

		/* ADC3 规则通道转换 */
		ADC_RegularChannelConfig(ADC3, ADC_Channel_11, 1, ADC_SampleTime_1Cycles5);

		/* 使能 ADC3 外部触发转换 */
		ADC_ExternalTrigConvCmd(ADC3, ENABLE);

		/* 使能 ADC3 */
		ADC_Cmd(ADC3, ENABLE);

		/* Enable ADC3 DMA */
		ADC_DMACmd(ADC3, ENABLE);

		/* 使能 ADC3 复位校准寄存器 */
		ADC_ResetCalibration(ADC3);
		/* 检测复位校准寄存器 */
		while(ADC_GetResetCalibrationStatus(ADC3));

		/* 开始 ADC3 校准 */
		ADC_StartCalibration(ADC3);
		/* 等待校准结束 */
		while(ADC_GetCalibrationStatus(ADC3));
	}
	#endif

	/* 配置采样触发定时器，使用TIM1 CC1 */
	SetSampRate(g_DSO.SampleFreq);	/* 修改采样频率(启动时100K) */
}

/*
*********************************************************************************************************
*	函 数 名: SetSampRate
*	功能说明: 修改采样频率
*	形    参：freq : 采样频率 单位Hz
*	返 回 值: 无
*********************************************************************************************************
*/
static void SetSampRate(uint32_t freq)
{
	/* 第4步： 配置定时器2第2通道CC2, CC2作为ADC1的触发源 */
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint16_t Period;

	TIM_Cmd(TIM1, DISABLE);

	/*
	采样频率计算公式 ：
		period = 72000000 / freq ;
		
		1200 == 60KHz 采样频率
	*/
	Period = 72000000 / freq;		

	/* 使能 TIM1 时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
	TIM_TimeBaseStructure.TIM_Period = Period;          
	TIM_TimeBaseStructure.TIM_Prescaler = 0;   /* 计数频率 = 72000 000 / 18 = 4000 000	 */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/* TIM1 通道1配置为PWM模式 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;                
	TIM_OCInitStructure.TIM_Pulse = TIM_TimeBaseStructure.TIM_Period / 2; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;         
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);

	/* TIM1 计数器使能 */
	TIM_Cmd(TIM1, ENABLE);

	/* TIM1 输出使能 */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);  
}





/*
*********************************************************************************************************
*	函 数 名: DispCh1Wave
*	功能说明: 显示通道1波形
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispCh1Wave(void)
{
	int16_t i;		/* 有符号数 */
	uint16_t *px;
	uint16_t *py;
	int16_t iTemp;

	static uint8_t s_DispFirst = 0;		/* 用于第一次调用时刷屏 */

	/* 显示通道1电平标记, 一个短的水平横线，在波形最左边 */
	{
		static uint16_t y = 70;

		LCD_DrawLine(1, y, 6, y, CL_BLUE);

		y = g_DSO.Ch1VOffset;

		if (y < 20)
		{
			y = 20;
		}
		else if (y > 220)
		{
			y = 220;
		}
		LCD_DrawLine(1, y, 6, y, CL_YELLOW);
	}

	if (s_DispFirst == 0)
	{
		/* 只在进入的时候清一次屏，以后不再清屏，否则会有严重的闪烁感 */
		s_DispFirst = 1;
		LCD_ClrScr(CL_BLUE);  	/* 清屏，背景蓝色 */
	}

	if (g_DSO.BufUsed == 0)
	{
		g_DSO.BufUsed = 1;
	}
	else
	{
		g_DSO.BufUsed = 0;
	}

	if (g_DSO.BufUsed == 0)
	{
		px = g_DSO.xCh1Buf1;
		py = g_DSO.yCh1Buf1;
	}
	else
	{
		px = g_DSO.xCh1Buf2;
		py = g_DSO.yCh1Buf2;
	}

	/* CH1通道, 输入0V时，ADC采样值 = 1872 ,这是根据多个开发板求到的平均值 */
	for (i = 0; i < 300; i++)
	{
		px[i] = 10 + i;
		iTemp = g_DSO.Ch1VOffset + (int16_t)((1872 - g_DSO.Ch1Buf[i + 1]) * 10) / g_DSO.Ch1Attenuation;

		if (iTemp > 220)
		{
			iTemp = 220;
		}
		else if (iTemp < 20)
		{
			iTemp = 20;
		}
		py[i] = iTemp;
	}

	/* 下面的代码采取清除上帧波形，然后再重新描绘新波形，避免整屏刷新的闪烁感 */
	/* 清除上帧波形 */
	if (g_DSO.BufUsed == 0)
	{
		LCD_DrawPoints(g_DSO.xCh1Buf2, g_DSO.yCh1Buf2, 300, CL_BLUE);
	}
	else
	{
		LCD_DrawPoints(g_DSO.xCh1Buf1, g_DSO.yCh1Buf1, 300, CL_BLUE);
	}

	/* 显示更新的波形 */
	LCD_DrawPoints((uint16_t *)px, (uint16_t *)py, 300, CL_YELLOW);
}
/*
*********************************************************************************************************
*	函 数 名: InitDsoParam
*	功能说明: 初始化全局参数变量
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void InitDsoParam(void)
{
	g_DSO.Ch1Attenuation = 23;	/* 设置缺省衰减系数 */
	g_DSO.Ch2Attenuation = 23;	/* 设置缺省衰减系数 */
	g_DSO.Ch1VScale = 1000;	/* 缺省是1V */
	g_DSO.Ch2VScale = 1000;	/* 缺省是1V */

	g_DSO.Ch1VOffset = 70; 	/* 通道1 GND线位置 */
	g_DSO.Ch2VOffset = 170; /* 通道2 GND线位置 */

	g_DSO.ActiveCH = 1;		/* 缺省是CH1 */
	g_DSO.AdjustMode = 1;	/* 缺省是调节垂直偏移， 可以切换到2调节幅度 */

	g_DSO.HoldEn = 0; 		/* 暂停刷屏标志 */

	g_DSO.TimeBaseId = 0;	/* 时间分度查询表索引 */								 
	g_DSO.TimeBase = 	g_TimeTable[g_DSO.TimeBaseId];	 /* 缺省时间分度 */
	g_DSO.SampleFreq = 25000000 / g_DSO.TimeBase;  /* 计算采样频率 */


	g_DSO.Ch1AttId = 2;
	g_DSO.Ch1Attenuation = g_AttTable[g_DSO.Ch1AttId][0];
	g_DSO.Ch1VScale =  g_AttTable[g_DSO.Ch1AttId][1];

	g_DSO.Ch2AttId = 2;
	g_DSO.Ch2Attenuation = g_AttTable[g_DSO.Ch2AttId][0];
	g_DSO.Ch2VScale =  g_AttTable[g_DSO.Ch2AttId][1];

}

/*
*********************************************************************************************************
*	函 数 名: IncSampleFreq
*	功能说明: 增加采样频率，按 1-2-5 步进
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void IncSampleFreq(void)
{
	/*	时间轴分度（每1个大格的时长)
		20us 	      1250000
		50us 		   500000
		100us		   250000
		200us		   125000
		500us		    50000
		1ms				 2500
		2ms				 1250
		5ms				  500 
		10ms			  250
		20ms			  125
		50ms			   50
		100ms			   25
	*/
	if (g_DSO.TimeBaseId < (sizeof(g_TimeTable) / 4) - 1)
	{
		g_DSO.TimeBaseId++;
	}
													 
	g_DSO.TimeBase = 	g_TimeTable[g_DSO.TimeBaseId];
	g_DSO.SampleFreq = 25000000 / g_DSO.TimeBase;

	SetSampRate(g_DSO.SampleFreq);	/* 改变采样频率 */
}

/*
*********************************************************************************************************
*	函 数 名: DecSampleFreq
*	功能说明: 降低采样频率，按 1-2-5 步进
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DecSampleFreq(void)
{
	if (g_DSO.TimeBaseId > 0)
	{
		g_DSO.TimeBaseId--;
	}
													 
	g_DSO.TimeBase = g_TimeTable[g_DSO.TimeBaseId];
	g_DSO.SampleFreq = 25000000 / g_DSO.TimeBase;

	SetSampRate(g_DSO.SampleFreq);	/* 改变采样频率 */
}

/*
*********************************************************************************************************
*	函 数 名: AdjustAtt
*	功能说明: 调节波形显示幅度
*	形    参：	ch   : 通道号，1或2
*				mode : 0 降低， 1增加
*	返 回 值: 无
*********************************************************************************************************
*/
static void AdjustAtt(uint8_t ch, uint8_t mode)
{

	if (ch == 1)
	{
		if (mode == 0) 	/* 降低 */
		{
			if (g_DSO.Ch1AttId > 0)
			{
				g_DSO.Ch1AttId--;
			}
		}
		else		/* 增加 */
		{
			if (g_DSO.Ch1AttId < ATT_COUNT - 1)
			{
				g_DSO.Ch1AttId++;
			}
		}

		g_DSO.Ch1Attenuation = g_AttTable[g_DSO.Ch1AttId][0];
		g_DSO.Ch1VScale =  g_AttTable[g_DSO.Ch1AttId][1];
	}
	else if (ch == 2)
	{
		if (mode == 0) 	/* 降低 */
		{
			if (g_DSO.Ch2AttId > 0)
			{
				g_DSO.Ch2AttId--;
			}
		}
		else		/* 增加 */
		{
			if (g_DSO.Ch2AttId < ATT_COUNT - 1)
			{
				g_DSO.Ch2AttId++;
			}
		}
		g_DSO.Ch2Attenuation = g_AttTable[g_DSO.Ch2AttId][0];
		g_DSO.Ch2VScale =  g_AttTable[g_DSO.Ch2AttId][1];
	}
}

/*
*********************************************************************************************************
*	函 数 名: StopADC
*	功能说明: 暂停ADC采样
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void StopADC(void)
{
	TIM_Cmd(TIM1, DISABLE);

	/* 禁止 DMA1 通道1 */
	DMA_Cmd(DMA1_Channel1, DISABLE);
	
	/* 禁止 DMA2 通道5 */
	DMA_Cmd(DMA2_Channel5, DISABLE);
}

/*
*********************************************************************************************************
*	函 数 名: StartADC
*	功能说明: 重新启动ADC采样
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void StartADC(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	DMA_DeInit(DMA1_Channel1);		/* 复位DMA1寄存器到缺省状态 */
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	/* 选择ADC1的数据寄存器作为源 */
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_DSO.Ch1Buf;	/* 目标地址 */
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	/* 设置DMA传输方向，外设(ADC)作为源 */
	DMA_InitStructure.DMA_BufferSize = SAMPLE_COUNT;	/* 设置缓冲区大小 */
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	/* 外设地址不自增 */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	/* 存储器地址需要自增 */
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	/* 选择外设传输单位：16bit */
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	/* 选择内存传输单位：16bit */
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	/* 无需循环模式   */
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	/* 选择DMA优先级 */
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	/* DMA传输类型，不是内存到内存 */
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	DMA_DeInit(DMA2_Channel5);		/* 复位DMA2寄存器到缺省状态 */
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC3_DR_Address;	/* 选择ADC1的数据寄存器作为源 */
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_DSO.Ch2Buf;	/* 目标地址 */
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	/* 设置DMA传输方向，外设(ADC)作为源 */
	DMA_InitStructure.DMA_BufferSize = SAMPLE_COUNT;	/* 设置缓冲区大小 */
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	/* 外设地址不自增 */
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	/* 存储器地址需要自增 */
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	/* 选择外设传输单位：16bit */
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	/* 选择内存传输单位：16bit */
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	/* 无需循环模式   */
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;	/* 选择DMA优先级 */
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;	/* DMA传输类型，不是内存到内存 */
	DMA_Init(DMA2_Channel5, &DMA_InitStructure);

	/* 使能 DMA2 通道1 */
	DMA_Cmd(DMA1_Channel1, ENABLE);

	/* 使能 DMA2 通道5 */
	DMA_Cmd(DMA2_Channel5, ENABLE);

	TIM_Cmd(TIM1, ENABLE);
}

/*
*********************************************************************************************************
*	函 数 名: DsoRun
*	功能说明: DSO全速运行状态
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DsoRun(uint8_t *pMainStatus)
{
	uint8_t KeyCode;
	uint8_t fRefresh = 1;	/* LCD刷新标志 */
	


	LCD_ClrScr(CL_BLUE);  		/* 清屏，背景蓝色 */

	bsp_StartTimer(1, 150);		/* 启动定时器1，100ms刷新1次 */
	while (*pMainStatus == S_RUN)
	{
		CPU_IDLE();

		if (fRefresh)
		{
			fRefresh = 0;

			if (g_DSO.HoldEn == 1)		
			{
				DispDSO();
			}
		}

		if (bsp_CheckTimer(1))
		{
			bsp_StartTimer(1, 150);		/* 启动定时器1，100ms刷新1次 */

			/* 运行状态。每隔100ms刷新1次波形 */
			if (g_DSO.HoldEn == 0)
			{
			
			}
		}

		/* 读取按键，大于0表示有键按下 */
		KeyCode = bsp_GetKey();
		if (KeyCode > 0)
		{
			/* 有键按下 */
			switch (KeyCode)
			{
				case  KEY_DOWN_TAMPER:	/* TAMPER 键，通道选择(CH1或CH2) */
					if (g_DSO.ActiveCH == 1)
					{
						g_DSO.ActiveCH = 2;
					}
					else
					{
						g_DSO.ActiveCH = 1;
					}
					fRefresh = 1;		/* 请求刷新LCD */
					break;

				case  KEY_DOWN_WAKEUP:	/* WAKEUP 键, 调节模式选择(幅度或者垂直偏移) */
					/* 退出,进入全速运行状态 */
					*pMainStatus = S_HELP;
					break;

				case  KEY_DOWN_USER:	/* USER 键 */
					if (g_DSO.HoldEn == 0)
					{
						g_DSO.HoldEn = 1;

						/* 保存暂停时的时基,为了水平扩展用 */
						g_DSO.TimeBaseIdHold = g_DSO.TimeBaseId;	

						StopADC();
					}
					else
					{
						g_DSO.HoldEn = 0;
						StartADC();				
					}
					fRefresh = 1;		/* 请求刷新LCD */
					break;

				case KEY_DOWN_JOY_LEFT:	/* 摇杆LEFT键按下 */
					if (g_DSO.HoldEn == 0)
					{
						DecSampleFreq();	/* 递减采样频率 */
						fRefresh = 1;		/* 请求刷新LCD */
					}
					else
					{
						; /* 波形水平移动，待完善 */
					}
					break;

				case KEY_DOWN_JOY_RIGHT:	/* 摇杆RIGHT键按下 */
					if (g_DSO.HoldEn == 0)
					{
						IncSampleFreq();  	/* 递增采样频率 */					
						fRefresh = 1;		/* 请求刷新LCD */
					}
					else
					{
						; /* 波形水平移动，待完善 */
					}
					break;

				case  KEY_DOWN_JOY_OK:	/* 摇杆OK键 */
					if (g_DSO.AdjustMode == 0)
					{
						g_DSO.AdjustMode = 1;
					}
					else
					{
						g_DSO.AdjustMode = 0;
					}
					fRefresh = 1;		/* 请求刷新LCD */
					break;

				case KEY_DOWN_JOY_UP:	/* 摇杆UP键按下 */
					if (g_DSO.ActiveCH == 1) 	/* 当前激活的是CH1 */
					{
						if (g_DSO.AdjustMode == 0)	/* 调节幅度放大倍数 */
						{
							AdjustAtt(1, 1);
						}
						else 	/* 调节上下偏移 */
						{
							g_DSO.Ch1VOffset -= 5;
						}
					}
					else	/* 当前激活的是CH2 */
					{
						if (g_DSO.AdjustMode == 0)	/* 调节幅度放大倍数 */
						{
							AdjustAtt(2, 1);
						}
						else 	/* 调节上下偏移 */
						{
							g_DSO.Ch2VOffset -= 5;
						}
					}
					fRefresh = 1;		/* 请求刷新LCD */
					break;

				case KEY_DOWN_JOY_DOWN:		/* 摇杆DOWN键按下 */
					if (g_DSO.ActiveCH == 1) 	/* 当前激活的是CH1 */
					{
						if (g_DSO.AdjustMode == 0)	/* 调节幅度放大倍数 */
						{
							AdjustAtt(1, 0);
						}
						else 	/* 调节上下偏移 */
						{
							g_DSO.Ch1VOffset += 5;
						}
					}
					else	/* 当前激活的是CH2 */
					{
						if (g_DSO.AdjustMode == 0)	/* 调节幅度放大倍数 */
						{
							AdjustAtt(2, 0);
						}
						else 	/* 调节上下偏移 */
						{
							g_DSO.Ch2VOffset += 5;
						}
					}
					fRefresh = 1;		/* 请求刷新LCD */
					break;

				default:
					break;
			}
		}
	}
}
