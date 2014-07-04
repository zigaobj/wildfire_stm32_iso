
#include "stm32f10x.h"
#include "bsp_usart2_485.h"
#include "bsp_usart1.h"

/*
 * 函数名：main
 * 描述  ：主函数
 * 输入  ：无
 * 输出  ：无
 本工程为主机，实验时先给从机上电
 */
int main(void)
{
  uint8_t temp = 0;     

  /* USART2 config */
  USART2_Config();  //串口2用于收发485的数据
  USART1_Config();  //串口1用于输出调试信息
  printf("\r\n485实验主机\r\n");

  while(1)
  {
    GPIO_SetBits(GPIOB,GPIO_Pin_5); //进入发送模式
    USART_SendData(USART2, temp);	 //发送数据
    while (USART_GetFlagStatus(USART2,USART_FLAG_TC) !=SET);//轮询直到发送数据完毕	
    printf("\r\n发送数据成功！\r\n"); //使用串口1打印调试信息到终端

    GPIO_ResetBits(GPIOB,GPIO_Pin_5);	//进入接收模式
    while(	USART_GetFlagStatus(USART2,USART_FLAG_RXNE)!= SET); //轮询直到485接收到数据

    temp = USART_ReceiveData(USART2);
    printf("接收到的数据为%d \r\n",temp);				
  }
}


