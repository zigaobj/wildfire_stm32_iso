/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   2.4g无线模块/nrf24l01+/master 测试
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_spi_nrf.h"

/*
 * PA2  -  PG8   ce使能
 * PA1  -  PG15  cs片选
 * PA3  -  PC4   irq中断
 */
u8 status;	//用于判断接收/发送状态
u8 txbuf[4]={0,1,2,3};	 //发送缓冲
u8 rxbuf[4];			 //接收缓冲
int i=0;

 /**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)                  
{   
  SPI_NRF_Init();

  /* 串口1初始化 */
  USART1_Config();

  printf("\r\n 这是一个 NRF24L01 无线传输实验 \r\n");
  printf("\r\n 这是无线传输 主机端 的反馈信息\r\n");
  printf("\r\n   正在检测NRF与MCU是否正常连接。。。\r\n");

  /*检测NRF模块与MCU的连接*/
  status = NRF_Check(); 

  /*判断连接状态*/  
  if(status == SUCCESS)	   
    printf("\r\n      NRF与MCU连接成功！\r\n");  
  else	  
    printf("\r\n  NRF与MCU连接失败，请重新检查接线。\r\n");

  while(1)
  {
    printf("\r\n 主机端 进入自应答发送模式\r\n"); 
    NRF_TX_Mode();

    /*开始发送数据*/	
    status = NRF_Tx_Dat(txbuf);	  

    /*判断发送状态*/
    switch(status)
    {
      case MAX_RT:
        printf("\r\n 主机端 没接收到应答信号，发送次数超过限定值，发送失败。 \r\n");
      break;

      case ERROR:
        printf("\r\n 未知原因导致发送失败。 \r\n");
      break;

      case TX_DS:
        printf("\r\n 主机端 接收到 从机端 的应答信号，发送成功！ \r\n");	 		
      break;  								
    }			   	

    printf("\r\n 主机端 进入接收模式。 \r\n");	
    NRF_RX_Mode();

    /*等待接收数据*/
    status = NRF_Rx_Dat(rxbuf);

    /*判断接收状态*/
    switch(status)
    {
      case RX_DR:
      for(i=0;i<4;i++)
      {					
        printf("\r\n 主机端 接收到 从机端 发送的数据为：%d \r\n",rxbuf[i]);
        txbuf[i] =rxbuf[i];
      }
      break;

      case ERROR:
        printf("\r\n 主机端 接收出错。   \r\n");
      break;  		
    }
  }// while(1)
}
/*********************************************END OF FILE**********************/
