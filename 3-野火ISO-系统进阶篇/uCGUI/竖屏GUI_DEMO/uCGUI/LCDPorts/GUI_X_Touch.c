/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              礐/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUI_TOUCH_X.C
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "GUI_X.h"
#include "stm32f10x.h"
#include "bsp_touch.h"


extern uint16_t XPT2046_ReadAdc(uint8_t _ucCh);


void GUI_TOUCH_X_ActivateX(void) {
}

void GUI_TOUCH_X_ActivateY(void) {
}


int  GUI_TOUCH_X_MeasureX(void) 
{
  	unsigned char t=0,t1,count=0;
	unsigned short int databuffer[15]={5,7,9,3,2,6,4,0,3,1,0,0,0,0,0};//数据组
	unsigned short temp=0,X=0;	
 
    while(count<15)	//循环读数10次
	{	   	  
		databuffer[count]= XPT2046_ReadAdc(0xd0);
		count++;  
	}  
	if(count==15)//一定要读到10次数据,否则丢弃
	{  
	    do//将数据X升序排列
		{	
			t1=0;		  
			for(t=0;t<count-1;t++)
			{
				if(databuffer[t]>databuffer[t+1])//升序排列
				{
					temp=databuffer[t+1];
					databuffer[t+1]=databuffer[t];
					databuffer[t]=temp;
					t1=1; 
				}  
			}
		}while(t1); 	    		 	 		  
		X=(databuffer[6]+databuffer[7]+databuffer[8])/3;	  
 
	}
	return(4096 - X); 
}

int  GUI_TOUCH_X_MeasureY(void) {
	unsigned char t=0,t1,count=0;
	unsigned short int databuffer[15]={5,7,9,3,2,6,4,0,3,1,0,0,0,0,0};//数据组
	unsigned short temp=0,Y=0;	
 	
	while(count<15)//循环读数10次
	{	   	  
		databuffer[count]= XPT2046_ReadAdc(0x90);
		count++; 
	}  
	if(count==15)//一定要读到10次数据,否则丢弃
	{  
	    do//将数据X升序排列
		{	
			t1=0;		  
			for(t=0;t<count-1;t++)
			{
				if(databuffer[t]>databuffer[t+1])//升序排列
				{
					temp=databuffer[t+1];
					databuffer[t+1]=databuffer[t];
					databuffer[t]=temp;
					t1=1; 
				}  
			}
		}while(t1); 	    		 	 		  
		Y=(databuffer[6]+databuffer[7]+databuffer[8])/3;	  


	}
	return(4096-Y); 	/*200为校正补偿值*/
}
