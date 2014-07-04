/*
*********************************************************************************************************
*
*	模块名称 : UCGUI接口函数
*	文件名称 : bsp_ili9341_ucgui.c
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
#include "LCD_Private.h"      /* private modul definitions & config */
#include "GUI_Private.h"
#include "GUIDebug.h"
#include "bsp_ili9341_lcd.h"


#define BKCOLOR LCD_BKCOLORINDEX
#define   COLOR LCD_COLORINDEX


int LCD_L0_Init(void)
{  
	return 0;
}

void LCD_L0_SetOrg(int x,int y)
{

}

void LCD_L0_SetPixelIndex(int x, int y, int PixelIndex)
{
	SetPoint(x,y,PixelIndex);
}

unsigned int LCD_L0_GetPixelIndex(int x, int y)
{
	return GetPoint(x,y);
}

void LCD_L0_XorPixel(int x, int y)
{
	LCD_PIXELINDEX Index = GetPoint(x,y);
	SetPoint(x,y,LCD_NUM_COLORS-1-Index);
}

void LCD_L0_DrawHLine  (int x0, int y,  int x1)
{
	LCD9341_DrawHLine(x0,y,x1,LCD_COLORINDEX);
}

void LCD_L0_DrawVLine  (int x, int y0,  int y1)
{
  	LCD9341_DrawVLine(x,y0,y1,LCD_COLORINDEX);
}

void LCD_L0_FillRect(int x0, int y0, int x1, int y1) {
//	#if !LCD_SWAP_XY
//	  for (; y0 <= y1; y0++) {
//	    LCD_L0_DrawHLine(x0,y0, x1);
//	  }
//	#else
//	  for (; x0 <= x1; x0++) {
//	    LCD_L0_DrawVLine(x0,y0, y1);
//	  }
//	#endif

	LCD9341_FillRect(x0,y0,x1,y1,LCD_COLORINDEX);
}

void DrawBitLine1BPP(int x, int y, U8 const*p, int Diff, int xsize, const LCD_PIXELINDEX*pTrans)
{
  LCD_PIXELINDEX pixels;
  LCD_PIXELINDEX Index0 = *(pTrans+0);
  LCD_PIXELINDEX Index1 = *(pTrans+1);
/*
// Jump to right entry point
*/
  pixels = *p;
  switch (GUI_Context.DrawMode & (LCD_DRAWMODE_TRANS|LCD_DRAWMODE_XOR)) {
  case 0:
    #if defined (SETNEXTPIXEL)   /* Optimization ! */
      x+=Diff;
      SetCursor(x,y);
    #endif
    switch (Diff&7) {
    case 0:   
      goto WriteBit0;
    case 1:   
      goto WriteBit1;
    case 2:
      goto WriteBit2;
    case 3:
      goto WriteBit3;
    case 4:
      goto WriteBit4;
    case 5:   
      goto WriteBit5;
    case 6:   
      goto WriteBit6;
    case 7:   
      goto WriteBit7;
    }
    break;
  case LCD_DRAWMODE_TRANS:
    switch (Diff&7) {
    case 0:
      goto WriteTBit0;
    case 1:
      goto WriteTBit1;
    case 2:
      goto WriteTBit2;
    case 3:
      goto WriteTBit3;
    case 4:
      goto WriteTBit4;
    case 5:   
      goto WriteTBit5;
    case 6:   
      goto WriteTBit6;
    case 7:   
      goto WriteTBit7;
    }
    break;
  case LCD_DRAWMODE_XOR:
    switch (Diff&7) {
    case 0:   
      goto WriteXBit0;
    case 1:   
      goto WriteXBit1;
    case 2:
      goto WriteXBit2;
    case 3:
      goto WriteXBit3;
    case 4:
      goto WriteXBit4;
    case 5:   
      goto WriteXBit5;
    case 6:   
      goto WriteXBit6;
    case 7:   
      goto WriteXBit7;
    }
  }
/*
        Write with transparency
*/
  WriteTBit0:
    if (pixels&(1<<7)) SetPoint(x+0, y, Index1);
    if (!--xsize)
      return;
  WriteTBit1:
    if (pixels&(1<<6)) SetPoint(x+1, y, Index1);
    if (!--xsize)
      return;
  WriteTBit2:
    if (pixels&(1<<5)) SetPoint(x+2, y, Index1);
    if (!--xsize)
      return;
  WriteTBit3:
    if (pixels&(1<<4)) SetPoint(x+3, y, Index1);
    if (!--xsize)
      return;
  WriteTBit4:
    if (pixels&(1<<3)) SetPoint(x+4, y, Index1);
    if (!--xsize)
      return;
  WriteTBit5:
    if (pixels&(1<<2)) SetPoint(x+5, y, Index1);
    if (!--xsize)
      return;
  WriteTBit6:
    if (pixels&(1<<1)) SetPoint(x+6, y, Index1);
    if (!--xsize)
      return;
  WriteTBit7:
    if (pixels&(1<<0)) SetPoint(x+7, y, Index1);
    if (!--xsize)
      return;
    x+=8;
    pixels = *(++p);
    goto WriteTBit0;

/*
        Write without transparency
*/

  WriteBit0:
    SetPoint(x+0, y, (pixels&(1<<7)) ? Index1 : Index0);
    if (!--xsize)
      return;
  WriteBit1:
    SetPoint(x+1, y, (pixels&(1<<6)) ? Index1 : Index0);
    if (!--xsize)
      return;
  WriteBit2:
    SetPoint(x+2, y, (pixels&(1<<5)) ? Index1 : Index0);
    if (!--xsize)
      return;
  WriteBit3:
    SetPoint(x+3, y, (pixels&(1<<4)) ? Index1 : Index0);
    if (!--xsize)
      return;
  WriteBit4:
    SetPoint(x+4, y, (pixels&(1<<3)) ? Index1 : Index0);
    if (!--xsize)
      return;
  WriteBit5:
    SetPoint(x+5, y, (pixels&(1<<2)) ? Index1 : Index0);
    if (!--xsize)
      return;
  WriteBit6:
    SetPoint(x+6, y, (pixels&(1<<1)) ? Index1 : Index0);
    if (!--xsize)
      return;
  WriteBit7:
    SetPoint(x+7, y, (pixels&(1<<0)) ? Index1 : Index0);
    if (!--xsize)
      return;
    x+=8;
    pixels = *(++p);
    goto WriteBit0;

/*
        Write XOR mode
*/
  WriteXBit0:
    if (pixels&(1<<7))
      LCD_L0_XorPixel(x+0, y);
    if (!--xsize)
      return;
  WriteXBit1:
    if (pixels&(1<<6))
      LCD_L0_XorPixel(x+1, y);
    if (!--xsize)
      return;
  WriteXBit2:
    if (pixels&(1<<5))
      LCD_L0_XorPixel(x+2, y);
    if (!--xsize)
      return;
  WriteXBit3:
    if (pixels&(1<<4))
      LCD_L0_XorPixel(x+3, y);
    if (!--xsize)
      return;
  WriteXBit4:
    if (pixels&(1<<3))
      LCD_L0_XorPixel(x+4, y);
    if (!--xsize)
      return;
  WriteXBit5:
    if (pixels&(1<<2))
      LCD_L0_XorPixel(x+5, y);
    if (!--xsize)
      return;
  WriteXBit6:
    if (pixels&(1<<1))
      LCD_L0_XorPixel(x+6, y);
    if (!--xsize)
      return;
  WriteXBit7:
    if (pixels&(1<<0))
      LCD_L0_XorPixel(x+7, y);
    if (!--xsize)
      return;
    x+=8;
    pixels = *(++p);
    goto WriteXBit0;
}

static void  DrawBitLine2BPP(int x, int y, U8 const*p, int Diff, int xsize, const LCD_PIXELINDEX*pTrans) {
  LCD_PIXELINDEX pixels;
/*
// Jump to right entry point
*/
  pixels = *p;
  if (pTrans) {
    /*
      with palette
    */
    if (GUI_Context.DrawMode & LCD_DRAWMODE_TRANS) switch (Diff&3) {
    case 0:
      goto WriteTBit0;
    case 1:
      goto WriteTBit1;
    case 2:
      goto WriteTBit2;
    default:
      goto WriteTBit3;
    } else switch (Diff&3) {
    case 0:
      goto WriteBit0;
    case 1:
      goto WriteBit1;
    case 2:
      goto WriteBit2;
    default:
      goto WriteBit3;
    }
  /*
          Write without transparency
  */
  WriteBit0:
    SetPoint(x+0, y, *(pTrans+(pixels>>6)));
    if (!--xsize)
      return;
  WriteBit1:
    SetPoint(x+1, y, *(pTrans+(3&(pixels>>4))));
    if (!--xsize)
      return;
  WriteBit2:
    SetPoint(x+2, y, *(pTrans+(3&(pixels>>2))));
    if (!--xsize)
      return;
  WriteBit3:
    SetPoint(x+3, y, *(pTrans+(3&(pixels))));
    if (!--xsize)
      return;
    pixels = *(++p);
    x+=4;
    goto WriteBit0;
  /*
          Write with transparency
  */
  WriteTBit0:
    if (pixels&(3<<6))
      SetPoint(x+0, y, *(pTrans+(pixels>>6)));
    if (!--xsize)
      return;
  WriteTBit1:
    if (pixels&(3<<4))
      SetPoint(x+1, y, *(pTrans+(3&(pixels>>4))));
    if (!--xsize)
      return;
  WriteTBit2:
    if (pixels&(3<<2))
      SetPoint(x+2, y, *(pTrans+(3&(pixels>>2))));
    if (!--xsize)
      return;
  WriteTBit3:
    if (pixels&(3<<0))
      SetPoint(x+3, y, *(pTrans+(3&(pixels))));
    if (!--xsize)
      return;
    pixels = *(++p);
    x+=4;
    goto WriteTBit0;
  } else { 
    /* 
      without palette 
    */
    if (GUI_Context.DrawMode & LCD_DRAWMODE_TRANS) switch (Diff&3) {
    case 0:
      goto WriteDDPTBit0;
    case 1:
      goto WriteDDPTBit1;
    case 2:
      goto WriteDDPTBit2;
    default:
      goto WriteDDPTBit3;
    } else switch (Diff&3) {
    case 0:
      goto WriteDDPBit0;
    case 1:
      goto WriteDDPBit1;
    case 2:
      goto WriteDDPBit2;
    default:
      goto WriteDDPBit3;
    }
  /*
          Write without transparency
  */
  WriteDDPBit0:
    SetPoint(x+0, y, (pixels>>6));
    if (!--xsize)
      return;
  WriteDDPBit1:
    SetPoint(x+1, y, (3&(pixels>>4)));
    if (!--xsize)
      return;
  WriteDDPBit2:
    SetPoint(x+2, y, (3&(pixels>>2)));
    if (!--xsize)
      return;
  WriteDDPBit3:
    SetPoint(x+3, y, (3&(pixels)));
    if (!--xsize)
      return;
    pixels = *(++p);
    x+=4;
    goto WriteDDPBit0;
  /*
          Write with transparency
  */
  WriteDDPTBit0:
    if (pixels&(3<<6))
      SetPoint(x+0, y, (pixels>>6));
    if (!--xsize)
      return;
  WriteDDPTBit1:
    if (pixels&(3<<4))
      SetPoint(x+1, y, (3&(pixels>>4)));
    if (!--xsize)
      return;
  WriteDDPTBit2:
    if (pixels&(3<<2))
      SetPoint(x+2, y, (3&(pixels>>2)));
    if (!--xsize)
      return;
  WriteDDPTBit3:
    if (pixels&(3<<0))
      SetPoint(x+3, y, (3&(pixels)));
    if (!--xsize)
      return;
    pixels = *(++p);
    x+=4;
    goto WriteDDPTBit0;
  }
}

static void  DrawBitLine4BPP(int x, int y, U8 const*p, int Diff, int xsize, const LCD_PIXELINDEX*pTrans)
{
  LCD_PIXELINDEX pixels;

  pixels = *p;
  if (pTrans)
  {
    if (GUI_Context.DrawMode & LCD_DRAWMODE_TRANS)
    {
      if ((Diff&1) ==0)
        goto WriteTBit0;
        goto WriteTBit1;
    }
    else
    {
      if ((Diff&1) ==0)
        goto WriteBit0;
        goto WriteBit1;
    }

  WriteBit0:
    SetPoint(x+0, y, *(pTrans+(pixels>>4)));
    if (!--xsize)
      return;
  WriteBit1:
    SetPoint(x+1, y, *(pTrans+(pixels&0xf)));
    if (!--xsize)
      return;
    x+=2;
    pixels = *(++p);
    goto WriteBit0;
  /*
          Write with transparency
  */
  WriteTBit0:
    if (pixels>>4)
      SetPoint(x+0, y, *(pTrans+(pixels>>4)));
    if (!--xsize)
      return;
  WriteTBit1:
    if (pixels&0xf)
      SetPoint(x+1, y, *(pTrans+(pixels&0xf)));
    if (!--xsize)
      return;
    x+=2;
    pixels = *(++p);
    goto WriteTBit0;
  } else {
    /*
      without palette
    */
    if (GUI_Context.DrawMode & LCD_DRAWMODE_TRANS) {
      if ((Diff&1) ==0)
        goto WriteDDPTBit0;
      goto WriteDDPTBit1;
    } else {
      if ((Diff&1) ==0)
        goto WriteDDPBit0;
      goto WriteDDPBit1;
    }
  /*
          Write without transparency
  */
  WriteDDPBit0:
    SetPoint(x+0, y, (pixels>>4));
    if (!--xsize)
      return;
  WriteDDPBit1:
    SetPoint(x+1, y, (pixels&0xf));
    if (!--xsize)
      return;
    x+=2;
    pixels = *(++p);
    goto WriteDDPBit0;
  /*
          Write with transparency
  */
  WriteDDPTBit0:
    if (pixels>>4)
      SetPoint(x+0, y, (pixels>>4));
    if (!--xsize)
      return;
  WriteDDPTBit1:
    if (pixels&0xf)
      SetPoint(x+1, y, (pixels&0xf));
    if (!--xsize)
      return;
    x+=2;
    pixels = *(++p);
    goto WriteDDPTBit0;
  }
}

void DrawBitLine8BPP(int x, int y, U8 const*p, int xsize, const LCD_PIXELINDEX*pTrans) {
  LCD_PIXELINDEX pixel;
  if ((GUI_Context.DrawMode & LCD_DRAWMODE_TRANS)==0) {
    if (pTrans) {
      for (;xsize > 0; xsize--,x++,p++) {
        pixel = *p;
        SetPoint(x, y, *(pTrans+pixel));
      }
    } else {
      for (;xsize > 0; xsize--,x++,p++) {
        SetPoint(x, y, *p);
      }
    }
  } else {   /* Handle transparent bitmap */
    if (pTrans) {
      for (; xsize > 0; xsize--, x++, p++) {
        pixel = *p;
        if (pixel) {
          SetPoint(x+0, y, *(pTrans+pixel));
        }
      }
    } else {
      for (; xsize > 0; xsize--, x++, p++) {
        pixel = *p;
        if (pixel) {
          SetPoint(x+0, y, pixel);
        }
      }
    }
  }
}

void DrawBitLine16BPP(int x, int y, U16 const*p, int xsize)
{
  LCD_PIXELINDEX Index;
  if ((GUI_Context.DrawMode & LCD_DRAWMODE_TRANS)==0)
  {
//    for (;xsize > 0; xsize--,x++,p++)
//    {
//      SetPoint(x, y, *p);
//    }
		LCD_open_windows(x, y, xsize, 1);
		for (;xsize > 0; xsize--,p++)
		{
			ILI9341_RAM = *p;
		}
  }
  else
  {   /* Handle transparent bitmap */
    for (; xsize > 0; xsize--, x++, p++)
    {
      Index = *p;
      if (Index)
      {
        SetPoint(x+0, y, Index);
      }
    }
  }
}

void LCD_L0_DrawBitmap   (int x0, int y0,
                       int xsize, int ysize,
                       int BitsPerPixel, 
                       int BytesPerLine,
                       const U8* pData, int Diff,
                       const LCD_PIXELINDEX* pTrans)
{
  int i;
  switch (BitsPerPixel)
  {
  case 1:
    for (i=0; i<ysize; i++)
    {
      DrawBitLine1BPP(x0, i+y0, pData, Diff, xsize, pTrans);
      pData += BytesPerLine;
    }
    break;
  case 2:
    for (i=0; i<ysize; i++)
    {
      DrawBitLine2BPP(x0, i+y0, pData, Diff, xsize, pTrans);
      pData += BytesPerLine;
    }
    break;
  case 4:
    for (i=0; i<ysize; i++)
    {
      DrawBitLine4BPP(x0, i+y0, pData, Diff, xsize, pTrans);
      pData += BytesPerLine;
    }
    break;
  case 8:
    for (i=0; i<ysize; i++)
    {
      DrawBitLine8BPP(x0, i+y0, pData, xsize, pTrans);
      pData += BytesPerLine;
    }
    break;
  case 16:
    for (i=0; i<ysize; i++)
    {
      DrawBitLine16BPP(x0, i+y0, (U16*)pData, xsize);
      pData += BytesPerLine;
    }
    break;
  }
}


void LCD_L0_SetLUTEntry(U8 Pos, LCD_COLOR color){}
void LCD_L0_On(void){}
void * LCD_L0_GetDevFunc(int Index) {
  GUI_USE_PARA(Index);
  return 0;
}

