/* SimSun_21.c - SimSun_21 font for UCGUI font driver */ 
/********************************************************************************************************
*                                            UCGUI
*                                       http://www.ucgui.com
*
*                             (c) Copyright 2005-2006, UCGUI专业论坛, 学习交流UCGUI
*
*                               	ucgui@16.com 2005.07.31
*                                           作者: ucgui
*********************************************************************************************************
*/

#include "GUI.H" 
#ifndef GUI_FLASH 
	#define GUI_FLASH 
#endif 
 
extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_32;

/* SimSun_24.c - SimSun_24 font for UCGUI font driver */ 
/********************************************************************************************************
*                                            UCGUI
*                                       http://www.ucgui.com
*
*                             (c) Copyright 2005-2006, UCGUI专业论坛, 学习交流UCGUI
*
*                               	ucgui@16.com 2005.07.31
*                                           作者: ucgui
*********************************************************************************************************
*/

#include "..\core\GUI.H" 
#ifndef GUI_FLASH 
	#define GUI_FLASH 
#endif 
extern GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_24;

GUI_FLASH  const unsigned char acFontHZSimSun_24_c3c7[72] = {	/* 们 0xc3c7*/
________,________,________,
________,________,________,
______X_,__X_____,________,
______X_,___X____,________,
_____XX_,____X___,____XX__,
_____X__,____XX__,XXXXXX__,
____XX__,_X__X___,____X___,
____X___,_X______,____X___,
____XX__,_X______,____X___,
___X_X__,_X______,____X___,
___X_X__,_X______,____X___,
__X__X__,_X______,____X___,
_X___X__,_X______,____X___,
_____X__,_X______,____X___,
_____X__,_X______,____X___,
_____X__,_X______,____X___,
_____X__,_X______,____X___,
_____X__,_X______,____X___,
_____X__,_X______,____X___,
_____X__,_X______,____X___,
_____X__,_X______,____X___,
_____X__,_X______,_XXXX___,
_____X__,_X______,___X____,
________,________,________,
};

GUI_FLASH  const unsigned char acFontHZSimSun_24_ced2[72] = {	/* 我 0xced2*/
________,________,________,
________,________,________,
________,__X__X__,________,
________,XXX__XX_,_X______,
____XXXX,X____X__,_XX_____,
___X___X,_____X__,__XX____,
_______X,_____X__,__XX____,
_______X,_____X__,________,
_______X,_____XX_,____XX__,
__XXXXXX,XXXXXXXX,XXXX____,
_______X,_____XX_,__X_____,
_______X,_____XX_,__XX____,
_______X,_____XX_,__XX____,
_______X,_XXXXXX_,_XX_____,
______XX,X_____X_,XX______,
__XXXX_X,______X_,X_______,
__XX___X,______XX,X_______,
_______X,______XX,________,
_______X,_____XXX,X____X__,
_______X,___XX___,XX___X__,
_______X,__X_____,_XX__X__,
____XXXX,_X______,__XXXX__,
______X_,________,____XXX_,
________,________,________,
};

GUI_FLASH const GUI_CHARINFO GUI_FontHZSimSun_24_CharInfo[] = {
{ 24, 24, 3, (void GUI_FLASH *)&acFontHZSimSun_24_c3c7},/* 们 96 */
{ 24, 24, 3, (void GUI_FLASH *)&acFontHZSimSun_24_ced2},/* 我 97 */
};

/*我--我*/

GUI_FLASH const GUI_FONT_PROP GUI_FontHZSimSun_24_Propced2= {
      0xced2,
      0xced2,
      &GUI_FontHZSimSun_24_CharInfo[1],
      (void *)0 
};
/*们--们*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZSimSun_24_Propc3c7= {
      0xc3c7,
      0xc3c7,
      &GUI_FontHZSimSun_24_CharInfo[0],
      (void *)&GUI_FontHZSimSun_24_Propced2 
};
GUI_FLASH const GUI_FONT GUI_FontHZ_SimSun_32 = {
      GUI_FONTTYPE_PROP_SJIS, 
      24, 
      24, 
      8,  
      8,  
      (void GUI_FLASH *)&GUI_FontHZSimSun_24_Propc3c7
};

