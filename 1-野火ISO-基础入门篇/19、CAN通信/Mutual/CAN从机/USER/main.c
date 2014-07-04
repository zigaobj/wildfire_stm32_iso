/******************** (C) COPYRIGHT 2012 WildFire Team **************************
 * 文件名  ：main.c
 * 描述    ：双CAN通讯实验，这是主机部分，向从机发送ABCD         
 * 实验平台：野火STM32开发板
 * 库版本  ：ST3.5.0
 *
 * 作者    ：wildfire team 
 * 论坛    ：http://www.amobbs.com/forum-1008-1.html
 * 淘宝    ：http://firestm32.taobao.com
**********************************************************************************/
#include "stm32f10x.h"
#include "usart1.h"
#include "can.h"

__IO uint32_t flag = 0xff;		//用于标志是否接收到数据，在中断函数中赋值
CanTxMsg TxMessage;			    //发送缓冲区
CanRxMsg RxMessage; 			//接收缓冲区

/* 
 * 函数名：main
 * 描述  : “从机”的主函数
 * 输入  ：无
 * 输出  : 无
 */
int main(void)
{

	/* USART1 config */
	USART1_Config();
	
	/* 配置CAN模块 */
	CAN_Config();

	printf( "\r\n***** 这是一个双CAN通讯实验******** \r\n");
	printf( "\r\n 这是 “从机端” 的反馈信息： \r\n"); 
	
	/*等待主机端的数据*/	
	while( flag == 0xff );
	
	printf( "\r\n 成功接收到“主机”返回的数据\r\n ");	
	printf("\r\n 接收到的报文为：\r\n"); 
	printf("\r\n 扩展ID号ExtId：0x%x",RxMessage.ExtId);	 
	printf("\r\n 数据段的内容:Data[0]= 0x%x ，Data[1]=0x%x \r\n",RxMessage.Data[0],RxMessage.Data[1]);					

	/*设置要通过CAN发送的信息*/ 
	CAN_SetMsg();			

	 printf("\r\n将要发送的报文内容为：\r\n");
	 printf("\r\n 扩展ID号ExtId：0x%x",TxMessage.ExtId);
	 printf("\r\n 数据段的内容:Data[0]=0x%x ，Data[1]=0x%x \r\n",TxMessage.Data[0],TxMessage.Data[1]);

	/*发送消息 “CDAB”**/
	CAN_Transmit(CAN1, &TxMessage);		

  while(1);

}
/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/


