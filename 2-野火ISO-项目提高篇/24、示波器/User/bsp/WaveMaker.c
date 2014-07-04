#include "stm32f10x.h"
#include <stdio.h>
#include "math.h"
#include "WaveMaker.h"
#include "dso.h"
/*******************************************************************************
	正弦波数据，12bit，1个周期128个点
	波形由PA4输出, PA4 用作 DAC_OUT1， DAC的参考电压 = 3.3V
*******************************************************************************/
const uint16_t g_SineWave128[] = {
	2047 ,
	2147 ,
	2248 ,
	2347 ,
	2446 ,
	2544 ,
	2641 ,
	2737 ,
	2830 ,
	2922 ,
	3012 ,
	3099 ,
	3184 ,
	3266 ,
	3346 ,
	3422 ,
	3494 ,
	3564 ,
	3629 ,
	3691 ,
	3749 ,
	3803 ,
	3852 ,
	3897 ,
	3938 ,
	3974 ,
	4006 ,
	4033 ,
	4055 ,
	4072 ,
	4084 ,
	4092 ,
	4094 ,
	4092 ,
	4084 ,
	4072 ,
	4055 ,
	4033 ,
	4006 ,
	3974 ,
	3938 ,
	3897 ,
	3852 ,
	3803 ,
	3749 ,
	3691 ,
	3629 ,
	3564 ,
	3494 ,
	3422 ,
	3346 ,
	3266 ,
	3184 ,
	3099 ,
	3012 ,
	2922 ,
	2830 ,
	2737 ,
	2641 ,
	2544 ,
	2446 ,
	2347 ,
	2248 ,
	2147 ,
	2047 ,
	1947 ,
	1846 ,
	1747 ,
	1648 ,
	1550 ,
	1453 ,
	1357 ,
	1264 ,
	1172 ,
	1082 ,
	995  ,
	910  ,
	828  ,
	748  ,
	672  ,
	600  ,
	530  ,
	465  ,
	403  ,
	345  ,
	291  ,
	242  ,
	197  ,
	156  ,
	120  ,
	88   ,
	61   ,
	39   ,
	22   ,
	10   ,
	2    ,
	0    ,
	2    ,
	10   ,
	22   ,
	39   ,
	61   ,
	88   ,
	120  ,
	156  ,
	197  ,
	242  ,
	291  ,
	345  ,
	403  ,
	465  ,
	530  ,
	600  ,
	672  ,
	748  ,
	828  ,
	910  ,
	995  ,
	1082 ,
	1172 ,
	1264 ,
	1357 ,
	1453 ,
	1550 ,
	1648 ,
	1747 ,
	1846 ,
	1947
};
/* 正弦波 */
const uint16_t g_SineWave32[32] = {
                      2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056, 4095, 4056,
                      3939, 3750, 3495, 3185, 2831, 2447, 2047, 1647, 1263, 909,
                      599, 344, 155, 38, 0, 38, 155, 344, 599, 909, 1263, 1647};

/* 方波 */
const uint16_t g_SineWave32_FangBo[32] = {
                      0, 0, 0, 0, 0, 0, 0, 0,  
					  0, 0, 0, 0, 0, 0, 0, 0, 
                      4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095,
					  4095, 4095, 4095, 4095, 4095, 4095, 4095, 4095
};
uint16_t g_Wave[128];
/*******************************************************************************
*	函数名: InitWaveMaker
*	输  入:	无
*	输  出:	无
*	功能说明：初始化波形发生器。
*			  配置PA4 为DAC_OUT1, 启用DMA2 如果用于音频输出频率 分频等于0时
			  TIM_SetAutoreload(TIM6, 370); =72000000/370 如果是波形输出还要考虑
			  DMA的传输数据 比如DMA_BufferSize = 32 则=72000000/370/32
			  不知道为什么去掉单个的中括号会存在问题？？？ 别的工程不存在这个问题
*******************************************************************************/
void InitWaveMaker(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		
		
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
				
		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;			/* 配置PA4为DAC_OUT1 */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* 决定DAC输出的采样频率
		  x = 72000000 / 频率
		  ARR自动重装载寄存器周期的值(定时时间）
		*/													  
		TIM_PrescalerConfig(TIM6, 0, TIM_PSCReloadMode_Update); //预分频=0 
		
		TIM_SetAutoreload(TIM6, 50);	            /* 22 输出100KHz的正弦波 */
		TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

	 {
	   	DAC_InitTypeDef DAC_InitStructure;	
	    /* DAC channel1 Configuration */
		DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;               //定时器6 TRGO触发						 
		DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;	   //噪声三角波生成使能关闭
		DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;	   //输出缓冲关闭
//		DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
		DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	 }	
	 {	
	    DMA_InitTypeDef DMA_InitStructure;	
		DMA_DeInit(DMA2_Channel3);
		DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12R1_Address;
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&g_Wave;
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						     //存储器到外设
		DMA_InitStructure.DMA_BufferSize = 128;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;						    //循环模式
		DMA_InitStructure.DMA_Priority = DMA_Priority_High ;
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
		DMA_Init(DMA2_Channel3, &DMA_InitStructure);
		DMA_Cmd(DMA2_Channel3, ENABLE);
	}
	 /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is
	 automatically connected to the DAC converter. */
	 DAC_Cmd(DAC_Channel_1, ENABLE);		 //DAC通道1使能
	 /* Enable DMA for DAC Channel1 */
	 DAC_DMACmd(DAC_Channel_1, ENABLE);		 //DAC通道1DMA开启
}
/*************************************************************************************
设置DAC_CR寄存器的MAMPx[3:0]位可以屏蔽部分或者全部LFSR的数据，这样的得到的LSFR值与
DAC_DHRx的数值相加，去掉溢出位之后即被写入DAC_DORx寄存器
*************************************************************************************/
void InitNoise()
{  
  DAC_InitTypeDef            DAC_InitStructure;
  GPIO_InitTypeDef           GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  TIM_PrescalerConfig(TIM6, 0, TIM_PSCReloadMode_Update); //预分频=0 
  TIM_SetAutoreload(TIM6, 220);	 
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Noise;
  DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bits11_0;  //可以通过此处屏蔽的位数来设置幅值
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  DAC_Cmd(DAC_Channel_1, ENABLE);

  DAC_SetChannel1Data(DAC_Align_12b_R, 0);//可以通过这里设置对齐方式 并且通过后面的数据设置初始值
  TIM_Cmd(TIM6, ENABLE);					
}
 /*******************************************************************************
*	函数名: InitTriangle
*	输  入:	无
*	输  出:	无
*	功能说明：产生三角波
*			  
*******************************************************************************/
void InitTriangle()
{  
  DAC_InitTypeDef            DAC_InitStructure;
  GPIO_InitTypeDef           GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  TIM_PrescalerConfig(TIM6, 0, TIM_PSCReloadMode_Update); //预分频=0 
  TIM_SetAutoreload(TIM6, 22);	 
  TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T6_TRGO;
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Triangle;
  DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_4095;
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);
  DAC_Cmd(DAC_Channel_1, ENABLE);

  DAC_SetChannel1Data(DAC_Align_12b_R, 0);//可以通过这里设置对齐方式 并且通过后面的数据设置初始值
  TIM_Cmd(TIM6, ENABLE);					
}
 /*******************************************************************************
*	函数名: SetupWaveType
*	输  入:	无
*	输  出:	无
*	功能说明：设置是产生正弦波还是产生方波
*			  
*******************************************************************************/
void SetupWaveType(uint8_t _type)
{
	/*
		计算波形幅度。
		DAC寄存器最大值 4096 对应3.3V

		我们期望获得2V的幅度，系数 = 0.606
		4096 * 0.606 = 2482
	*/
	TIM_Cmd(TIM6, DISABLE);
	switch (_type)
	{
		case WT_SIN:	/* 正弦波 */
			{
				uint32_t i;

				for (i = 0; i < 128; i++)
				{
					/* 调整正弦波幅度 */
					//g_Wave[i] = g_SineWave128[i]*0.606;
					g_Wave[i] = 2048+1024*sin(2*3.1415926f*127*i/128); 
				}

			}
			break;

		case WT_RECT:	/* 方波 */
			{
				uint32_t i;

				for (i = 0; i < 64; i++)
				{
					g_Wave[i] = 0;
				}

				for (i = 0; i < 64; i++)
				{
					g_Wave[i] = 2482;
				}
			}
			break;
	}
    TIM_Cmd(TIM6, ENABLE);
}
