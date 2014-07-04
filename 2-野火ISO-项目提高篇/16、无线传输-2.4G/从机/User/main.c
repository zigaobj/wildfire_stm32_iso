/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   2.4g无线模块/nrf24l01+/slave 测试
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

u8 status;		 //用于判断接收/发送状态
u8 txbuf[4];	 //发送缓冲
u8 rxbuf[4];	 //接收缓冲
u8 i; 

 /**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{      
  /* 串口1初始化 */
  USART1_Config(); 

  /*SPI接口初始化*/   
  SPI_NRF_Init(); 

  printf("\r\n 这是一个 NRF24L01 无线传输实验 \r\n");
  printf("\r\n 这是无线传输 从机端 的反馈信息\r\n");
  printf("\r\n   正在检测NRF与MCU是否正常连接。。。\r\n");

  /*检测NRF模块与MCU的连接*/
  status = NRF_Check();   		
  if(status == SUCCESS)	   
    printf("\r\n      NRF与MCU连接成功\r\n");  
  else	  
    printf("\r\n   正在检测NRF与MCU是否正常连接。。。\r\n");

  while(1)
  {  		 	
    printf("\r\n 从机端 进入接收模式\r\n"); 
    NRF_RX_Mode();

    /*等待接收数据*/
    status = NRF_Rx_Dat(rxbuf);

    /*判断接收状态*/
    if(status == RX_DR)
    {
      for(i=0;i<4;i++)
      {	
        printf("\r\n 从机端 接收到 主机端 发送的数据为：%d \r\n",rxbuf[i]); 
        /*把接收的数据+1后发送给主机*/
        rxbuf[i]+=1;	  
        txbuf[i] = rxbuf[i]; 
      }
    }   
    printf("\r\n 从机端 进入自应答发送模式\r\n"); 
    NRF_TX_Mode();

    /*不断重发，直至发送成功*/	  
    do
    { 	  
      status = NRF_Tx_Dat(txbuf);
    }while(status == MAX_RT);
  } 
}
/*********************************************END OF FILE**********************/
