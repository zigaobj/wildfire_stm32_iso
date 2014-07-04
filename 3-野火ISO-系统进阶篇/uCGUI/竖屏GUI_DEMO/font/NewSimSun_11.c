/* NewSimSun_11.c - NewSimSun_11 font for UCGUI font driver */ 
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
extern GUI_FLASH const GUI_FONT GUI_FontHZ_NewSimSun_11;

GUI_FLASH  const unsigned char acFontHZNewSimSun_11_b0ae[22] = {	/* 爱 0xb0ae*/
_______X,X_______,
__XXXXX_,X_______,
___X_X_X,________,
__XXXXXX,XX______,
_X__X___,X_______,
__XXXXXX,________,
____X___,X_______,
___XX__X,________,
__X__XX_,________,
_XXXX__X,XX______,
________,________,
};

GUI_FLASH  const unsigned char acFontHZNewSimSun_11_ced2[22] = {	/* 我 0xced2*/
______X_,________,
__XXX_XX,X_______,
___X__X_,X_______,
_XXXXXXX,XX______,
___X__X_,X_______,
___XXXX_,X_______,
_XXX__XX,________,
___X__X_,________,
___X_X_X,_X______,
__XXX___,XX______,
________,________,
};

GUI_FLASH  const unsigned char acFontHZNewSimSun_11_e6c3[22] = {	/* 婷 0xe6c3*/
__X___X_,________,
__X___X_,_X______,
__XXXX_X,X_______,
_X_X_XX_,X_______,
_X_X_X_X,X_______,
_X_XX_X_,_X______,
_X_XX___,_X______,
__XX_XXX,X_______,
_X__X__X,________,
_____XXX,________,
________,________,
};

GUI_FLASH const GUI_CHARINFO GUI_FontHZNewSimSun_11_CharInfo[] = {
{ 12, 12, 2, (void GUI_FLASH *)&acFontHZNewSimSun_11_b0ae},/* 爱 96 */
{ 12, 12, 2, (void GUI_FLASH *)&acFontHZNewSimSun_11_ced2},/* 我 97 */
{ 12, 12, 2, (void GUI_FLASH *)&acFontHZNewSimSun_11_e6c3},/* 婷 98 */
};

/*婷--婷*/

GUI_FLASH const GUI_FONT_PROP GUI_FontHZNewSimSun_11_Prope6c3= {
      0xe6c3,
      0xe6c3,
      &GUI_FontHZNewSimSun_11_CharInfo[2],
      (void *)0 
};
/*我--我*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZNewSimSun_11_Propced2= {
      0xced2,
      0xced2,
      &GUI_FontHZNewSimSun_11_CharInfo[1],
      (void *)&GUI_FontHZNewSimSun_11_Prope6c3 
};
/*爱--爱*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZNewSimSun_11_Propb0ae= {
      0xb0ae,
      0xb0ae,
      &GUI_FontHZNewSimSun_11_CharInfo[0],
      (void *)&GUI_FontHZNewSimSun_11_Propced2 
};
GUI_FLASH const GUI_FONT GUI_FontHZ_NewSimSun_11 = {
      GUI_FONTTYPE_PROP_SJIS, 
      12, 
      12, 
      1,  
      1,  
      (void GUI_FLASH *)&GUI_FontHZNewSimSun_11_Propb0ae
};
