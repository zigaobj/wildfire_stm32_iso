#include "stm32f10x.h"
#include "FFTInc.h"
#include "dso.h"
#include "stm32_dsp.h"
#include "math.h"
#include "GUI.h"
#include "DIALOG.h"
				

/******************************************************************************************
* 每个单元数据高字(高16位)中存储采样数据的实部，低字(低16位)存储采样数据的虚部(总是为0)
* 进行FFT变换，并计算各次谐波幅值
*******************************************************************************************/
void powerMag(void)					             //不同的采样点需要修改 这样速度快点
{
  int16_t lX,lY;
  uint16_t i;
  for (i=0; i < 512; i++)
  {
    lX= (g_DSO.FFTOUT[i]<<16)>>16;               /* sine_cosine --> cos */
    lY= (g_DSO.FFTOUT[i]>> 16);                  /* sine_cosine --> sin */    
    {
//    float X=  1024*((float)lX)/32768;
//    float Y = 1024*((float)lY)/32768;
//    float Mag = sqrt(X*X+ Y*Y)/1024;		 
	  float X= (float)lX/32;
      float Y = (float)lY/32;
	  float Mag = sqrt(X*X+ Y*Y)/1024;
      g_DSO.FFTMAG[i]= (uint32_t)(Mag*65536);		
    }    
  }
}

/*******************************************************************************
*	函数名: Audio_Spectrum(void)
*	输  入:	无
*	输  出:	无
*	功  能：显示通道1波形
*******************************************************************************/
static void Audio_Spectrum(void)
{
	int16_t i;									 /* 有符号数 */
	uint16_t *px;
	uint16_t *py;
	int16_t iTemp;
	static uint8_t s_DispFirst = 0;				/* 用于第一次调用时刷屏 */
	if (s_DispFirst == 0)
	{
		s_DispFirst = 1;
		for(i=0;i<128;i++)
		{
		 g_DSO.xCh2Buf1[i]=220;
		 g_DSO.xCh2Buf1[i]=220;
		 g_DSO.xCh2Buf2[i]=220;
		 g_DSO.xCh2Buf2[i]=220;
	    }	  					  
	}	
	if (g_DSO.BufUsed1 == 0)					  //设置两个缓冲器用于擦出上一帧波形
	{
		g_DSO.BufUsed1= 1;
	}
	else
	{
		g_DSO.BufUsed1 = 0;
	}

	if (g_DSO.BufUsed1 == 0)
	{
		px = g_DSO.xCh2Buf1;
		py = g_DSO.yCh2Buf1;
	}
	else
	{
		px = g_DSO.xCh2Buf2;
		py = g_DSO.yCh2Buf2;
	}
	for (i = 0; i < 61; i++)				
	{
		px[i] = 15 + i*4;	
		iTemp = g_DSO.Ch2VOffset -g_DSO.FFTMAG[i]/30;
	    if (iTemp < 20)
		{
			iTemp = 20;
		}
		py[i] = iTemp;
	}

//    GUI_GotoXY(323-55,72);
//	GUI_DispFloatFix(g_DSO.FFTMAG[0]/20000,5,2);			  //显示直流分量的大小     	
	if (g_DSO.BufUsed1 == 0)
	{	
	    for(i=0;i<61;i++)
		{
		   GUI_SetColor(GUI_BLUE);
		   GUI_FillRect(px[i],g_DSO.yCh2Buf2[i],px[i]+2,220);
		   GUI_SetColor(GUI_GREEN);
		   GUI_FillRect(px[i],py[i],px[i]+2,220);
		}
	}
	else
	{
	  for(i=0;i<61;i++)
		{
		   GUI_SetColor(GUI_BLUE);
		   GUI_FillRect(px[i],g_DSO.yCh2Buf1[i],px[i]+2,220);
		   GUI_SetColor(GUI_GREEN);
		   GUI_FillRect(px[i],py[i],px[i]+2,220);

		}
	}
}
/*******************************************************************************
*	函数名: DispWave_Spectrum
*	输  入:	无
*	输  出:	无
*	功  能：显示通道1波形
*******************************************************************************/
static void DispWave_Spectrum(void)
{
	int16_t i;									 /* 有符号数 */
	uint16_t *px;
	uint16_t *py;
	int16_t iTemp;
	static uint8_t s_DispFirst = 0;				/* 用于第一次调用时刷屏 */
	if (s_DispFirst == 0)
	{
		s_DispFirst = 1;
		for(i=0;i<128;i++)
		{
		 g_DSO.xCh2Buf1[i]=220;
		 g_DSO.xCh2Buf1[i]=220;
		 g_DSO.xCh2Buf2[i]=220;
		 g_DSO.xCh2Buf2[i]=220;
	    }	  					  
	}	
	if (g_DSO.BufUsed1 == 0)					  //设置两个缓冲器用于擦出上一帧波形
	{
		g_DSO.BufUsed1= 1;
	}
	else
	{
		g_DSO.BufUsed1 = 0;
	}

	if (g_DSO.BufUsed1 == 0)
	{
		px = g_DSO.xCh2Buf1;
		py = g_DSO.yCh2Buf1;
	}
	else
	{
		px = g_DSO.xCh2Buf2;
		py = g_DSO.yCh2Buf2;
	}
	
	for (i = 0; i < 100; i++)				
	{
		px[i] = 20 + i;
		
		iTemp = g_DSO.Ch2VOffset -g_DSO.FFTMAG[i]/20;
		
		if (iTemp > 200)
		{
			iTemp = 200;
		}
		else if (iTemp <0)
		{
			iTemp =0;
		}
		py[i] = iTemp;
	}
	  GUI_SetColor(GUI_MAGENTA);	
	
//      GUI_GotoXY(323-55,72);
//	  GUI_DispFloatFix(g_DSO.FFTMAG[0]/20000,5,2);			  //显示直流分量的大小                   
    	if (g_DSO.BufUsed1 == 0)
	{
		GUI_SetColor(GUI_BLUE);
	    GUI_DrawGraph((I16*)g_DSO.yCh2Buf2, 100, 20,20);
	}
	else
	{
		GUI_SetColor(GUI_BLUE);
	    GUI_DrawGraph((I16*)g_DSO.yCh2Buf1, 100, 20,20);			   
	}	
	    GUI_SetColor(GUI_YELLOW);
	    GUI_DrawGraph((I16*)py, 100, 20,20);
}

/*****************************************************************************************
*	函数名: FFT_Convert
*	输  入:	无
*	输  出:	无
*	功  能：实现FFT转化后的显示	   记得修改采样率!!!!!! 还要记得修改M 蝶形的级数
******************************************************************************************/
extern uint8_t SpectrumFlag;
void FFT_Convert(void)
{	
	static uint8_t flag1=1;			  //标志位用于一次初始化 主要是正弦和余弦函数
   	uint16_t i,Cycle;
	uint8_t flag;
	
	      for(i=0;i<1024;i++)
		{
		  g_DSO.FFTIN[i]=((int32_t)(g_DSO.Ch1Buf[i]-1900)* 14060) / 1050; //扩大了10000倍 总值不要超过65336
		  g_DSO.FFTIN[i]=g_DSO.FFTIN[i]<<16;
		}
		
		cr4_fft_1024_stm32(g_DSO.FFTOUT,g_DSO.FFTIN,1024);
		
		powerMag();
		
		Cycle=g_DSO.FFTMAG[1];
		flag=1;
		
		for(i=2;i<512;i++)
		{
		  if(Cycle<g_DSO.FFTMAG[i]){Cycle=g_DSO.FFTMAG[i];flag=i;}
		}
		GUI_SetColor(GUI_MAGENTA);
        GUI_DispDecAt(flag*g_DSO.SampleFreq*2/1024 ,323-55,136,6);	   //估计频率
		
	    if(SpectrumFlag == 0)
	   {
	     Audio_Spectrum();
	   }
	   else
	   {
         DispWave_Spectrum();
	   }
}

