/* @NewSimSun_15.c - @NewSimSun_15 font for UCGUI font driver */ 
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

#include"GUI.H" 
#ifndef GUI_FLASH 
	#define GUI_FLASH 
#endif 
extern GUI_FLASH const GUI_FONT GUI_FontHZ_@NewSimSun_15;

GUI_FLASH  const unsigned char acFontHZ@NewSimSun_15_b0ae[30] = {	/* 爱 0xb0ae*/
________,________,
____X___,________,
____XX_X,_____X__,
X_X_X_XX,_____X__,
X__XX__X,_X___X__,
X___X__X,_XX_X___,
_X__X__X,_X_X____,
_X_XX__X,_X_X____,
_XX_X__X,_XX_X___,
_X__X_XX,XX___X__,
_X_XX__X,__X__X__,
_XX_X__X,___X_X__,
_X__X__X,____X___,
____XX__,_____X__,
______X_,________,
};

GUI_FLASH  const unsigned char acFontHZ@NewSimSun_15_ced2[30] = {	/* 我 0xced2*/
________,________,
________,__XXXX__,
____X___,_____X__,
__XXX_XX,____X___,
_X__X___,X__X____,
____X___,_XX_____,
XXXXXXXX,X_X_____,
____X___,___X____,
_X__X__X,___X____,
_X__X__X,________,
_XXXXXXX,XXXXX___,
__X_X___,X____X__,
__X_X___,_X__X___,
__X_X___,_X______,
________,_X______,
};

GUI_FLASH  const unsigned char acFontHZ@NewSimSun_15_e6c3[30] = {	/* 婷 0xe6c3*/
________,________,
_X_____X,X_______,
_X_____X,_X______,
_X_XXX_X,_X______,
_X_X_X_X,_X______,
XX_X_X_X,_XXXX___,
_X_X_X_X,_X___X__,
_X_XXX_X,_X__X___,
_X_____X,_X______,
_X_____X,X___X___,
___XXXXX,XX_X____,
___X____,__X_____,
XXXX____,_X_X____,
___XXXX_,X___X___,
___X___X,_____X__,
};

GUI_FLASH const GUI_CHARINFO GUI_FontHZ@NewSimSun_15_CharInfo[] = {
{ 15, 15, 2, (void GUI_FLASH *)&acFontHZ@NewSimSun_15_b0ae},/* 爱 96 */
{ 15, 15, 2, (void GUI_FLASH *)&acFontHZ@NewSimSun_15_ced2},/* 我 97 */
{ 15, 15, 2, (void GUI_FLASH *)&acFontHZ@NewSimSun_15_e6c3},/* 婷 98 */
};

/*婷--婷*/

GUI_FLASH const GUI_FONT_PROP GUI_FontHZ@NewSimSun_15_Prope6c3= {
      0xe6c3,
      0xe6c3,
      &GUI_FontHZ@NewSimSun_15_CharInfo[2],
      (void *)0 
};
/*我--我*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ@NewSimSun_15_Propced2= {
      0xced2,
      0xced2,
      &GUI_FontHZ@NewSimSun_15_CharInfo[1],
      (void *)&GUI_FontHZ@NewSimSun_15_Prope6c3 
};
/*爱--爱*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ@NewSimSun_15_Propb0ae= {
      0xb0ae,
      0xb0ae,
      &GUI_FontHZ@NewSimSun_15_CharInfo[0],
      (void *)&GUI_FontHZ@NewSimSun_15_Propced2 
};
GUI_FLASH const GUI_FONT GUI_FontHZ_@NewSimSun_15 = {
      GUI_FONTTYPE_PROP_SJIS, 
      15, 
      15, 
      1,  
      1,  
      (void GUI_FLASH *)&GUI_FontHZ@NewSimSun_15_Propb0ae
};
