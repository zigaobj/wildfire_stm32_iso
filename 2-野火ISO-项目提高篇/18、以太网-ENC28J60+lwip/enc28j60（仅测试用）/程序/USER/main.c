#include "stm32f10x.h"
#include "usart1.h"
#include "led.h"
#include "spi_enc28j60.h"
#include "web_server.h"

/*
 * 函数名：main
 * 描述  ：主函数
 * 输入  ：无
 * 输出  ：无	
 */
int main (void)
{
  /* 配置系统时钟为72M */
	SystemInit();
  
	/* ENC28J60 SPI 接口初始化 */
  SPI_Enc28j60_Init();
	
	/* ENC28J60 WEB 服务程序 */  
  Web_Server();

}

