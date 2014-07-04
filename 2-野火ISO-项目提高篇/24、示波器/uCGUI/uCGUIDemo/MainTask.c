
#include <includes.h>
#include "bsp.h"
#include "GUI.h"
#include "DIALOG.h"
#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include "dso.h"
#include "FFTInc.h"

#include "WM.h"
#include "BUTTON.h"
#include "CHECKBOX.h"
#include "DROPDOWN.h"
#include "EDIT.h"
#include "FRAMEWIN.h"
#include "LISTBOX.h"
#include "MULTIEDIT.h"
#include "RADIO.h"
#include "SLIDER.h"
#include "TEXT.h"
#include "PROGBAR.h"
#include "SCROLLBAR.h"
#include "LISTVIEW.h"


/* 时间分度表， g_DSO.TimeBaseId作为数组索引  */
/* 每个点可以表示的最低像素，1/1.250000M       */
/* 每个点可以表示的最低像素，1/1.000000M       */
const uint32_t g_TimeTable[] =
{
					
//	20, 	50, 	100, 
//	200,	500,	1000,
//	2000,	5000,	10000,
//	20000,	50000,	100000,
//	200000,	500000,	1000000
					16,
	20, 	40, 	100, 
	200,	400,	1000,
	2000,	4000,	10000,
	20000,	40000,	100000,
	200000,	400000,	1000000	
			
};

/****************************************************************** 
  衰减倍数表  
   (1)每个方格对应的是25个点 表示 1V 
   (2)1.406V 对应ADC数值 1050  1050/1.406 =746.799;
    对应关系就是 25个点阵 == 1V == ADC计数的746.799
	由可得 1V的衰减因子是746.799/25 = 29.87
*******************************************************************/
#define ATT_COUNT	9
const uint32_t g_AttTable[ATT_COUNT][2] =
{
/* {除数,每大格电压}  1:1 除数放大了10倍 因为前面被除数放大了10倍*/
	{1450,	5000},
	{580,	2000},
	{290,	1000},
	{145,	500},
	{58,	200},
	{29,	100},
	{15,	50},
	{6,	    20},
	{3,	    10},
};

typedef struct
{
  unsigned char Hour;
  unsigned char Min;
  unsigned char Sec;
} Time;

#define DSO_VER			 "V1.0"		         /* 当前版本 */
#define GUI_ID_ADC       (GUI_ID_USER + 0)
#define GUI_ID_DAC  	 (GUI_ID_USER + 1)
#define GUI_ID_FFT   	 (GUI_ID_USER + 2)
#define GUI_ID_Spectrum  (GUI_ID_USER + 3)
#define GUI_ID_OSCLevel  (GUI_ID_USER + 4)


uint8_t fRefresh ;           //用于LCD屏的刷新 
DSO_T g_DSO;						/* 全局变量，是一个结构体 */
extern const GUI_BITMAP bmpsu;
uint8_t ConfigRefresh ;
unsigned long Clock_count;
Time     Clock;
extern OS_EVENT *Semp;
uint8_t SpectrumFlag =0;
BUTTON_Handle    ahButton[4];
PROGBAR_Handle   ahProgbar[1];
SCROLLBAR_Handle ahScrollbar[1];
/*
*********************************************************************************************************
*	函 数 名: InitDSO
*	功能说明: 对示波器通道1进行初始化配置。主要完成GPIO的配置、ADC的配置、DMA配置。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
 void InitDSO(void)
{
  	{
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC	| RCC_APB2Periph_AFIO, ENABLE); //配置IO口
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);			   			//使能定时器1时钟
		RCC_ADCCLKConfig(RCC_PCLK2_Div2);     										//ADCCLK = PCLK2/4 = 18MHz
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
	#if 1
	{
		DMA_InitTypeDef DMA_InitStructure;
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

		DMA_DeInit(DMA1_Channel1);													/* 复位DMA1寄存器到缺省状态 */
		DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;					/* 选择ADC1的数据寄存器作为源 */
		DMA_InitStructure.DMA_MemoryBaseAddr =(uint32_t)&g_DSO.buffer;						/* 目标地址 */
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							/* 设置DMA传输方向，外设(ADC)作为源 */
		DMA_InitStructure.DMA_BufferSize = SAMPLE_COUNT;							/* 设置缓冲区大小 */
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			/* 外设地址不自增 */
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						/* 存储器地址需要自增 */
//		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	/* 选择外设传输单位：16bit */
//		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			/* 选择内存传输单位：16bit */
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;	    /* 选择外设传输单位：16bit */
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;			    /* 选择内存传输单位：16bit */
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;								/* 无需循环模式   */
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;						/* 选择DMA优先级 */
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								/* DMA传输类型，不是内存到内存 */
		DMA_Init(DMA1_Channel1, &DMA_InitStructure);
		DMA_Cmd(DMA1_Channel1, ENABLE);
	}
	#endif
	#if 1
	{
		ADC_InitTypeDef	ADC_InitStructure;

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_ADC2, ENABLE);

		  ADC_DeInit(ADC1);
		  ADC_DeInit(ADC2);
		  /* ADC1 configuration ------------------------------------------------------*/
		  ADC_InitStructure.ADC_Mode = ADC_Mode_FastInterl;
		  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
		  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
		  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC3;
		  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		  ADC_InitStructure.ADC_NbrOfChannel = 1;
		  ADC_Init(ADC1, &ADC_InitStructure);
		  ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_1Cycles5);//18M/(1.5+12.5)=1.2857M最大采样频率  
		  ADC_ExternalTrigConvCmd(ADC1, ENABLE);
		  
		  /* ADC1 configuration ------------------------------------------------------*/
		  ADC_InitStructure.ADC_Mode = ADC_Mode_FastInterl;
		  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
		  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
		  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
		  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
		  ADC_InitStructure.ADC_NbrOfChannel = 1;
		  ADC_Init(ADC2, &ADC_InitStructure);
		  ADC_RegularChannelConfig(ADC2, ADC_Channel_10, 1, ADC_SampleTime_1Cycles5); //18M/(1.5+12.5)=1.2857M最大采样频率
		  ADC_ExternalTrigConvCmd(ADC2, ENABLE);
		  
		  /* Enable ADC1 */
		  ADC_DMACmd(ADC1, ENABLE);
		  ADC_Cmd(ADC1, ENABLE);  
		  ADC_ResetCalibration(ADC1);
		  while(ADC_GetResetCalibrationStatus(ADC1));
		  ADC_StartCalibration(ADC1);
		  while(ADC_GetCalibrationStatus(ADC1));
		     
		  /* Enable ADC2 */
		  ADC_Cmd(ADC2, ENABLE);
		  ADC_ResetCalibration(ADC2);
		  while(ADC_GetResetCalibrationStatus(ADC2));
		  ADC_StartCalibration(ADC2);
		  while(ADC_GetCalibrationStatus(ADC2));
	}
	#endif
	#if 0
	{
		ADC_InitTypeDef	ADC_InitStructure;

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
		ADC_InitStructure.ADC_ScanConvMode = DISABLE;
		ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;							/* 连续转换静止 */
		//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;		/* 选择TIM1的CC1做触发 */
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC3;		/* 选择TIM2的CC3做触发 */
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;						/* 数据右对齐,高位为0 */
		ADC_InitStructure.ADC_NbrOfChannel = 1;										/* 1个通道 */
		ADC_Init(ADC3, &ADC_InitStructure);		
		ADC_RegularChannelConfig(ADC3, ADC_Channel_11, 1, ADC_SampleTime_1Cycles5);	/* ADC1 regular channels configuration */
		ADC_ExternalTrigConvCmd(ADC3, ENABLE);
		ADC_Cmd(ADC3, ENABLE);
		ADC_DMACmd(ADC3, ENABLE);
		ADC_ResetCalibration(ADC3);
		while(ADC_GetResetCalibrationStatus(ADC3));
		ADC_StartCalibration(ADC3);
		while(ADC_GetCalibrationStatus(ADC3));
	}
	#endif
	//SetSampRate(g_DSO.SampleFreq);													/* 配置采样触发定时器，使用TIM1 CC3修改采样频率(启动时100K) */
	#if 1
	{   
	    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_OCInitTypeDef  TIM_OCInitStructure;
		TIM_Cmd(TIM1, DISABLE);	
		TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 				   //初始化定时器1的寄存器为复位值
		TIM_TimeBaseStructure.TIM_Period = 72000000 / g_DSO.SampleFreq;    //ARR自动重装载寄存器周期的值(定时时间）到设置频率后产生个更新或者中断(也是说定时时间到)
		TIM_TimeBaseStructure.TIM_Prescaler = 0;   						   //PSC时钟预分频数 例如：时钟频率=TIM1CLK/(时钟预分频+1)
		TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    				   //CR1->CKD时间分割值
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	   //CR1->CMS[1:0]和DIR定时器模式 向上计数
		TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				   //CCMR2在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为有效电平，否则为无效电平
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;      //CCER 输出使能          
		TIM_OCInitStructure.TIM_Pulse = TIM_TimeBaseStructure.TIM_Period / 2;//CCR3同计数器TIMx_CNT的比较，并在OC4端口上产生输出信号 
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;    	   //CCER输出极性设置	高电平有效     
		TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	    //TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);					//CMR2 设置预装载使能  更新事件产生时写入有效
		//TIM_ARRPreloadConfig(TIM1, ENABLE);		   		 				//CR1  设置ARR自动重装 更新事件产生时写入有效
		TIM_Cmd(TIM1, ENABLE);											   
		TIM_CtrlPWMOutputs(TIM1, ENABLE);  								   //使能PWM 输出
	}
	#endif
}

/*
*********************************************************************************************************
*	函 数 名: SetSampRate
*	功能说明: 修改采样频率
*	形    参：freq : 采样频率 单位Hz
*	返 回 值: 无
*********************************************************************************************************
*/
 void SetSampRate(uint32_t freq)
{
  TIM1->ARR=72000000 / freq;  									   //设定计数器自动重装值 
}
/*
*********************************************************************************************************
*	函 数 名: DispCh1Wave
*	功能说明: 显示通道1波形
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
 void DispCh1Wave(void)
{
	int16_t i,j = 0;		/* 有符号数 */
	uint16_t *px;
	uint16_t *py;
	int16_t iTemp;
    uint16_t VccMax,VccMin;
	uint16_t Average;

      for(i=0;i<1024;i++)
	{ 
		
		g_DSO.Ch1Buf[j++]=g_DSO.buffer[i]>>16;		//ADC2
		g_DSO.Ch1Buf[j++]=g_DSO.buffer[i] & 0xFFFF;	//ADC1
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

	  GUI_SetColor(GUI_MAGENTA);
	
	 /* 下面的代码用于实现 最大值和最小值得差值  */
	 VccMax=g_DSO.Ch1Buf[0];
	 VccMin=g_DSO.Ch1Buf[0];
	 for(i=1;i<1024;i++)
	 {
	    if(VccMax<g_DSO.Ch1Buf[i])VccMax=g_DSO.Ch1Buf[i];
		if(VccMin>g_DSO.Ch1Buf[i])VccMin=g_DSO.Ch1Buf[i];
	 }
	  VccMax=VccMax-VccMin;
      GUI_GotoXY(323-55,104);
	  GUI_DispFloatFix((VccMax* 1.406)/1050,5,2);
	 
	 /* CH1通道, 输入0V时，ADC采样值 = 1900 ,1024个点求到的平均值 */
	 /* 增益1:1的时候 1.406对应的AD值是2951 -1901 = 1050  */
	  for(i=1;i<21;i++)
	 {
	    Average +=g_DSO.Ch1Buf[i];
	 } 
	 Average = Average / 20;
	 GUI_GotoXY(323-55,40);
	 GUI_DispFloatFix(((Average-1900) * 1.406)/1050,5,2);
	 

	 GUI_DispDecAt(g_DSO.Ch1Buf[0], 323-55, 72, 4);
	/* 下面的代码用于实现波形的刷新  */
	for (i = 0; i < 300; i++)
	{
		px[i] = 15 + i;
		iTemp = g_DSO.Ch1VOffset + (int16_t)((1900 - g_DSO.Ch1Buf[i + 1]) * 10)/g_DSO.Ch1Attenuation ;
		if (iTemp > 200)
		{
			iTemp = 200;
		}
		else if (iTemp < 0)
		{
			iTemp = 0;
		}
		py[i] = iTemp;
	}

	/* 下面的代码采取清除上帧波形，然后再重新描绘新波形，避免整屏刷新的闪烁感 */
	/* 清除上帧波形 */
	if (g_DSO.BufUsed == 0)
	{
		GUI_SetColor(GUI_BLUE);
	    GUI_DrawGraph((signed short *)g_DSO.yCh1Buf2, 250, 14,20);

	}
	else
	{
		GUI_SetColor(GUI_BLUE);
	    GUI_DrawGraph((signed short *)g_DSO.yCh1Buf1, 250, 14,20);
	}

	/* 显示更新的波形 */
	GUI_SetColor(GUI_YELLOW);
	GUI_DrawGraph((I16 *)py, 250, 14,20);
}
/*
*********************************************************************************************************
*	函 数 名: InitDsoParam
*	功能说明: 初始化全局参数变量
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
 void InitDsoParam(void)
{				

	g_DSO.Ch1VOffset = 100; 				/* 通道1 GND线位置 */
	g_DSO.Ch2VOffset = 220;

	g_DSO.AdjustMode = 0;					/* 缺省是调节放大和衰减 */

	g_DSO.HoldEn = 0;
	g_DSO.TimeBaseId = 0;									 
	g_DSO.TimeBase   = g_TimeTable[g_DSO.TimeBaseId];
	g_DSO.SampleFreq = 25000000 / g_DSO.TimeBase;


	g_DSO.Ch1AttId = 2;
	g_DSO.Ch1Attenuation = g_AttTable[g_DSO.Ch1AttId][0];		/* 缺省是1V */
	g_DSO.Ch1VScale =  g_AttTable[g_DSO.Ch1AttId][1];			/* 设置缺省衰减系数 */

}

/*
*********************************************************************************************************
*	函 数 名: IncSampleFreq
*	功能说明: 增加采样频率，按 1-2-5 步进
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
 void IncSampleFreq(void)
{
	/*	时间轴分度（每1个大格的时长)
	    10us          2500000		
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
 void DecSampleFreq(void)
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
 void AdjustAtt(uint8_t ch, uint8_t mode)
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
 void StopADC(void)
{
		DMA1_Channel1->CCR&=0xffffffe;          					//关闭DMA传输
}


/*******************************************************************************
*	函数名: DispButton
*	输  入:	无
*	输  出:	无
*	功  能：显示波形窗口右边的功能按钮（待扩展）
*******************************************************************************/
 void DispButton(void)
{
	GUI_SetColor(GUI_GREEN);
				
	GUI_DispStringAt(  "VCC",          323-55, 24);
	GUI_DispStringAt(  "          V",   323-55, 40);
	GUI_DispStringAt(   "ADC",         323-55, 56);
	GUI_DispStringAt(  "          N",     323-55, 72);
    GUI_DispStringAt(   "VPP",         323-55, 88);
	GUI_DispStringAt(  "          V",   323-55, 104);
	GUI_DispStringAt(  "PERIOD",       323-55, 120);
	GUI_DispStringAt(  "           H", 323-55, 136);
}
/*******************************************************************************
*	函数名: DispChInfo
*	输  入:	无
*	输  出:	无
*	功  能：显示通道信息
*******************************************************************************/
static void DispChInfo(void)
{
	char buf[32];   							/* 字符显示缓冲区 */
											

	GUI_SetColor(GUI_GREEN);   					/* 白字 */
	GUI_DispStringAt( DSO_VER,15, 2);
		
	if (g_DSO.Ch1DC == 1)						/* 显示通道1信息 */
	{
		strcpy(buf, "CH1 ");
	}
	else
	{
		strcpy(buf, "CH1 ");
	}

	if (g_DSO.Ch1VScale >= 1000)
	{
		sprintf(&buf[4], "%d.00V/div", g_DSO.Ch1VScale / 1000);
	}
	else
	{
		sprintf(&buf[4], "%dmV/div", g_DSO.Ch1VScale);
	}
	GUI_DispStringAt( buf,15, 224);
	
	
	/* 显示时基 */
	if (g_DSO.TimeBase <= 1000)
	{
		sprintf(buf, "Time %3dus/div", g_DSO.TimeBase/2);
	}
	else if (g_DSO.TimeBase < 1000000)
	{
		sprintf(buf, "Time %3dms/div  ", g_DSO.TimeBase / 2000);
	}
	else
	{
		sprintf(buf, "Time %3ds/div   ", g_DSO.TimeBase / 2000000);	
	}
	GUI_DispStringAt(buf, 170, 224);
	                                                                                                                                                                                                                                             
	
	if (g_DSO.AdjustMode == 1)
	{
        GUI_DispStringAt("ChangeFreq   ",170, 2);
	}
	else
	{
		GUI_DispStringAt("ChangeScope ",170, 2);
	}
	
	
	sprintf(buf, "Freq:%7dHz  ",	g_DSO.SampleFreq*2);
    GUI_DispStringAt(buf,52, 2);
}
/*
*********************************************************************************************************
*	函 数 名: StartADC
*	功能说明: 重新启动ADC采样
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void StartADC(void)
{
	#if 1
	  DMA1_Channel1->CPAR=ADC1_DR_Address; 	 				    //DMA1 外设地址 
	  DMA1_Channel1->CMAR=(uint32_t)&g_DSO.buffer; 			    //DMA1,存储器地址
	  DMA1_Channel1->CNDTR=SAMPLE_COUNT;    					//DMA1,传输数据量
	  DMA1_Channel1->CCR=0X00000000;							//复位
	  DMA1_Channel1->CCR|=0<<4;  								//从外设来读
	  DMA1_Channel1->CCR|=0<<5;  								//普通模式
	  DMA1_Channel1->CCR|=0<<6;  								//外设地址非增量模式
	  DMA1_Channel1->CCR|=1<<7;  								//存储器增量模式
	  DMA1_Channel1->CCR|=2<<8;  								//外设数据宽度为32位
	  DMA1_Channel1->CCR|=2<<10; 								//存储器数据宽度32位
	  DMA1_Channel1->CCR|=2<<12; 								//高优先级
	  DMA1_Channel1->CCR|=0<<14; 								//非存储器到存储器模式	
   #endif

	  DMA1_Channel1->CCR|=1;          							//开启DMA传输
	  //TIM1->CR1|=1;									    		//开启定时器1
}
 void DispFrame(void)
{
	uint16_t x,y;
	GUI_SetColor(GUI_WHITE);					
	GUI_DrawRect(13, 19, 265, 221);						//这个函数对坐标值有影响 
	for (x = 0; x < 11; x++)							/* 绘制垂直刻度点 */
	{
		for (y = 0; y < 41; y++)
		{
			GUI_DrawPixel(14 + (x * 25), 20 + (y * 5));
		}
	}
	for (y = 0; y < 9; y++)								/* 绘制水平刻度点 */
	{
		for (x = 0; x < 51; x++)
		{
			GUI_DrawPixel(14 + (x * 5), 20 + (y * 25));
		}
	}
	for (y = 0; y < 41; y++)							/* 绘制垂直中心刻度点 */
	{	 
		GUI_DrawPixel(13 + (5 * 25), 20 + (y * 5));
		GUI_DrawPixel(15 + (5 * 25), 20 + (y * 5));
	}	
	for (x = 0; x < 51; x++)							/* 绘制水平中心刻度点 */
	{	 
		GUI_DrawPixel(14 + (x * 5), 19 + (4 * 25));
		GUI_DrawPixel(14 + (x * 5), 21 + (4 * 25));
	}
}

/*
*********************************************************************************************************
*	函 数 名: _cbBkWin
*	功能说明: 回调函数，最底层的，也就是桌面窗口
*	形    参：WM_MESSAGE* pMsg
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbBkWin(WM_MESSAGE* pMsg) {

  int NCode, Id;
  switch (pMsg->MsgId) {
 case WM_PAINT:
    break;
 case WM_NOTIFY_PARENT: 
     Id = WM_GetId(pMsg->hWinSrc); 
     NCode = pMsg->Data.v;
      switch (Id) {        
      case GUI_ID_ADC:
	      switch(NCode)
          {
            case WM_NOTIFICATION_RELEASED:
				if (g_DSO.AdjustMode == 0)		   /* 调节幅度放大倍数 */
				{
					AdjustAtt(1, 1);
				}
				else 							    /* 调节上下偏移 */
				{
					DecSampleFreq();  				/* 递增采样频率 */	
				}
				fRefresh = 1;				        /* 请求刷新LCD */
                 break;
          }
        break;				  
	  case GUI_ID_DAC:
	      switch(NCode)
          {
            case WM_NOTIFICATION_RELEASED:
				if (g_DSO.AdjustMode == 0)		   /* 调节幅度放大倍数 */
				{
					AdjustAtt(1, 0);
				}
				else 							    /* 调节上下偏移 */
				{
					IncSampleFreq();				   /* 递减采样频率 */
				}
				fRefresh = 1;				        /* 请求刷新LCD */
                break;
          }
        break;
	  case GUI_ID_FFT:   
	   	  switch(NCode)
          {
            case WM_NOTIFICATION_RELEASED:
				if (g_DSO.AdjustMode == 0)		   
				{
					g_DSO.AdjustMode = 1;
				}
				else
				{
					g_DSO.AdjustMode = 0;
				}
				fRefresh = 1;						/* 请求刷新LCD */
                 break;
          }
        break;
	  case GUI_ID_Spectrum:
	       switch(NCode)
	       {
	         case WM_NOTIFICATION_RELEASED:
                if(SpectrumFlag == 1)
				{
				   GUI_Clear();
				   DispButton();
				   SpectrumFlag = 0;
				   BUTTON_SetText(ahButton[3],"Spectr");
				}
				else
				{
				   GUI_Clear();
				   DispButton();
				   SpectrumFlag = 1;
				   BUTTON_SetText(ahButton[3],"Spetrum2");
				}   
	            break;
	       }
        break;
	   case GUI_ID_OSCLevel:
	         switch(NCode)
            {
                case WM_NOTIFICATION_VALUE_CHANGED:
                    g_DSO.Ch1VOffset = SCROLLBAR_GetValue(ahScrollbar[0]);
                    break;
            }
            break;   
      }
	 break;
 case WM_TOUCH:

     break;
 case WM_DELETE:

     break;
  default:
    WM_DefaultProc(pMsg);
  }
}
/*********************************************************************
*
*       MainTask
*
**********************************************************************
*/
void MainTask(void) 
{ 
    GUI_Init();
    WM_SetCreateFlags(WM_CF_MEMDEV);  /* Use memory devices on all windows to avoid flicker */
	WM_EnableMemdev(WM_HBKWIN);
	WM_SetCallback(WM_HBKWIN, &_cbBkWin);


	GUI_SetBkColor(GUI_BLUE);
	GUI_SetFont(&GUI_Font16B_1);
	GUI_Clear();


    ahButton[0] = BUTTON_Create(318-52,152,75-21, 20, GUI_ID_ADC,  BUTTON_CF_SHOW );
	BUTTON_SetTextColor(ahButton[0],BUTTON_CI_UNPRESSED,GUI_GREEN);
    BUTTON_SetFont(ahButton[0],&GUI_Font16B_1);
	BUTTON_SetText(ahButton[0],"Inc");
		
	ahButton[1] = BUTTON_Create(318-52,174,75-21, 20,  GUI_ID_DAC,  BUTTON_CF_SHOW );
	BUTTON_SetTextColor(ahButton[1],BUTTON_CI_UNPRESSED,GUI_GREEN);
    BUTTON_SetFont(ahButton[1],&GUI_Font16B_1);
	BUTTON_SetText(ahButton[1],"Dec");

	ahButton[2] = BUTTON_Create(318-52,196,75-21, 20,  GUI_ID_FFT,  BUTTON_CF_SHOW );
	BUTTON_SetTextColor(ahButton[2],BUTTON_CI_UNPRESSED,GUI_GREEN);
    BUTTON_SetFont(ahButton[2],&GUI_Font16B_1);
	BUTTON_SetText(ahButton[2],"Switch");

	ahButton[3] = BUTTON_Create(318-52,218,75-21, 20,  GUI_ID_Spectrum, BUTTON_CF_SHOW );
	BUTTON_SetTextColor(ahButton[3],BUTTON_CI_UNPRESSED,GUI_GREEN);
    BUTTON_SetFont(ahButton[3],&GUI_Font16B_1);
	BUTTON_SetText(ahButton[3],"Spectr");

	
	ahProgbar[0]=PROGBAR_Create(318-52,2,75-21,20,WM_CF_SHOW);
    PROGBAR_SetBarColor(ahProgbar[0],0,GUI_MAGENTA);
    PROGBAR_SetBarColor(ahProgbar[0],1,GUI_RED);
    PROGBAR_SetFont(ahProgbar[0],&GUI_Font16B_1);
	PROGBAR_SetMinMax(ahProgbar[0],0,100);
	WIDGET_SetEffect(ahProgbar[0], &WIDGET_Effect_3D);

	ahScrollbar[0]=SCROLLBAR_Create(0,11,12,217,0,GUI_ID_OSCLevel,WM_CF_SHOW,SCROLLBAR_CF_VERTICAL);
    SCROLLBAR_SetNumItems(ahScrollbar[0],200);
	SCROLLBAR_SetValue(ahScrollbar[0],100);
	
	DispButton();
	DispChInfo();
	ConfigRefresh=0;
	fRefresh =1;
	while(1)
	{ 
	   if(ConfigRefresh == 0) 
	   { 
		  if (fRefresh)
		  {
			  fRefresh = 0;
			  if(g_DSO.HoldEn == 1)
			{		 
			   DispChInfo();	
			}	 
			  
		  }
		  if (g_DSO.HoldEn == 0)
		  {
		    StopADC();	  			 /* 暂停采样 */			 
	 		DispFrame();
			DispChInfo();
			DispCh1Wave();
			FFT_Convert();
		    StartADC();	  			 /* 开始采样 */
		  }
		  PROGBAR_SetValue(ahProgbar[0],OSCPUUsage);
	   }
		GUI_Delay(50); 			 
	}
}





