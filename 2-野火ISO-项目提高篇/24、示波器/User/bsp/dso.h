/****************************************************************************
* Copyright (C), 2009-2010, www.armfly.com
*
* 文件名: dso.h
* 内容简述: 头文件
*
* 文件历史:
* 版本号  日期       作者    说明
* v0.1    2010-02-09 armfly  创建该文件
*
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DSO_H
#define __DSO_H

#define ADC1_DR_Address    ((uint32_t)0x4001244C)
#define ADC2_DR_Address    ((uint32_t)0x4001284C)
#define ADC3_DR_Address    ((uint32_t)0x40013C4C)

#define	SAMPLE_COUNT	(1024) 	/* 采样深度，即最大样本个数 */
/*
	示波器相关的数据结构
*/
typedef struct
{
	uint16_t Ch1Buf[SAMPLE_COUNT*2];	 /* 通道1数据缓冲区 */
	uint32_t buffer[SAMPLE_COUNT];
    int       FFTIN [1024];
	int	      FFTOUT[1024];
	long      FFTMAG[512];
	long      FFTPHASE[512];
	uint32_t TimeBaseIdHold;			/* 暂停时的时基 */

	uint32_t TimeBaseId;			/* 时基索引, 查表可得到 us单位的时基 */
	uint32_t SampleFreq;			/* 采样频率，单位Hz */
	uint32_t TimeBase;				/* 时基 查表可得到 us单位的时基 */

	uint8_t  Ch1AttId;				/* CH1 衰减倍数索引 */
	uint8_t  Ch2AttId;				/* CH2 衰减倍数索引 */
	int32_t  Ch1Attenuation; 		/* 波形1幅度衰减系数(原始数据x10后，再除以这个数)  */
	int32_t  Ch2Attenuation; 		/* 波形2幅度衰减系数(原始数据x10后，再除以这个数)  */
	uint16_t Ch1VScale;				/* 通道1垂直分度值mV单位 */
	uint16_t Ch2VScale;				/* 通道2垂直分度值mV单位 */

	uint32_t TriggerLevel;			/* 触发电平(ADC采样结果比较值) */
	uint32_t TriggerUpEdge;			/* 1表示触发模式上跳沿，0表示下跳沿 */

	uint8_t Ch1DC;					/* 0是交流 1表示CH1是直流 */
	uint8_t Ch2DC;					/* 0是交流 1表示CH2是直流 */   

	int16_t Ch1VOffset;				/* 通道1 GND线位置, 可以为负数 */
	int16_t Ch2VOffset;				/* 通道1 GND线位置, 可以为负数 */

	uint8_t ActiveCH;				/* 当前焦点通道 1表示CH1, 2表示CH2 */
	uint8_t AdjustMode;				/* 当前调节模式，0表示调节幅度，1表示调节偏移 */

	/* 使用2个缓冲区完成波形的擦除和重现 */
	uint16_t xCh1Buf1[310];		/* 波形数据，坐标数组 */
	uint16_t yCh1Buf1[310];		/* 波形数据，坐标数组 */
	uint16_t xCh1Buf2[310];		/* 波形数据，坐标数组 */
	uint16_t yCh1Buf2[310];		/* 波形数据，坐标数组 */

	uint16_t xCh2Buf1[256];		/* 波形数据，坐标数组 */
	uint16_t yCh2Buf1[256];		/* 波形数据，坐标数组 */
	uint16_t xCh2Buf2[256];		/* 波形数据，坐标数组 */
	uint16_t yCh2Buf2[256];		/* 波形数据，坐标数组 */
	uint8_t  BufUsed;			/* 0表示当前用Buf1， 1表示用Buf2 */
	uint8_t  BufUsed1;
	int16_t Ch1Changx;

	uint8_t HoldEn;				/* 波形采集暂停标志 1表示暂停，0表示运行 */
}DSO_T;

/* 
	定义支持的采样频率
*/
typedef enum
{
	SR_1K = 0,				/*  */

}SAMPLE_RATE_E;

void InitDSO(void);
void SetSampRate(uint32_t freq);
void Delay(uint32_t nCount);
void InitDsoParam(void);
void DispButton(void);
void DispDSO(void);
void StopADC(void);
void StartADC(void);
void AdjustAtt(uint8_t ch, uint8_t mode);
void DecSampleFreq(void);
void IncSampleFreq(void);
extern DSO_T g_DSO;

extern DSO_T g_DSO;
#endif


