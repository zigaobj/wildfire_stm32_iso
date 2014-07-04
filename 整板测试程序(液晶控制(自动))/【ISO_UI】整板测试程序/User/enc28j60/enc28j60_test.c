#include "enc28j60_test.h"

void ENC28J60_Test(void)
{
  /* ENC28J60 SPI 接口初始化 */
  SPI_Enc28j60_Init();
	
	/* ENC28J60 WEB 服务程序 */  
  Web_Server();
}

