#include "stm32f10x.h"
#include "bsp_sdio_sdcard.h"
#include "bsp_usart1.h"	

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;


#define BLOCK_SIZE            512 /* Block Size in Bytes */

#define NUMBER_OF_BLOCKS      32  /* For Multi Blocks operation (Read/Write) */
#define MULTI_BUFFER_SIZE    (BLOCK_SIZE * NUMBER_OF_BLOCKS)   //缓冲区大小	 


uint8_t Buffer_Block_Tx[BLOCK_SIZE], Buffer_Block_Rx[BLOCK_SIZE];
uint8_t readbuff[BLOCK_SIZE];
uint8_t Buffer_MultiBlock_Tx[MULTI_BUFFER_SIZE], Buffer_MultiBlock_Rx[MULTI_BUFFER_SIZE];
volatile TestStatus EraseStatus = FAILED, TransferStatus1 = FAILED, TransferStatus2 = FAILED;
SD_Error Status = SD_OK;
extern SD_CardInfo SDCardInfo;	
int i;
 
void SD_EraseTest(void);
void SD_SingleBlockTest(void);
void SD_MultiBlockTest(void);
void Fill_Buffer(uint8_t *pBuffer, uint32_t BufferLength, uint32_t Offset);
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength);
TestStatus eBuffercmp(uint8_t* pBuffer, uint32_t BufferLength);

int main(void)
{									   
	/* 中断初始化 */
	NVIC_Configuration();

	/* 串口初始化 */
	USART1_Config();	
	printf( "\r\n 这是一个SD卡 4bit dma 实验\r\n " );

	/* SD 卡初始化 */
	Status = SD_Init();
	
	if(Status == SD_OK) 
	{    
		printf( " \r\n SD_Init 初始化成功 \r\n " );		
	}
	else
	{
		printf("\r\n SD_Init 初始化失败 \r\n" );
		printf("\r\n 返回的Status的值为： %d \r\n",Status );
	}			  	

	printf( " \r\n CardType is ：%d ", SDCardInfo.CardType );
	printf( " \r\n CardCapacity is ：%d ", SDCardInfo.CardCapacity );
	printf( " \r\n CardBlockSize is ：%d ", SDCardInfo.CardBlockSize );
	printf( " \r\n RCA is ：%d ", SDCardInfo.RCA);
	printf( " \r\n ManufacturerID is ：%d \r\n", SDCardInfo.SD_cid.ManufacturerID );

	/* 擦除测试 */
	SD_EraseTest();
	
  /* 单块读写测试 */
	SD_SingleBlockTest(); 
	
	/* 多块读写测试 */	
	SD_MultiBlockTest();  

	while (1)
	{}
}


/*
 * 函数名：SD_EraseTest
 * 描述  ：擦除数据测试
 * 输入  ：无
 * 输出  ：无
 */
void SD_EraseTest(void)
{
  if (Status == SD_OK)
  {    
		/* 第一个参数为擦除起始地址，第二个参数为擦除结束地址 */
    Status = SD_Erase(0x00, (BLOCK_SIZE * NUMBER_OF_BLOCKS));
  }

  if (Status == SD_OK)
  {	/* 读取刚刚擦除的区域 */
    Status = SD_ReadMultiBlocks(Buffer_MultiBlock_Rx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS);

    /* 查询传输是否结束 */
    Status = SD_WaitReadOperation(); 
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }
 
  if (Status == SD_OK)
  {	/* 把擦除区域读出来对比 */
    EraseStatus = eBuffercmp(Buffer_MultiBlock_Rx, MULTI_BUFFER_SIZE);
  }
  
  if(EraseStatus == PASSED)
  	printf("\r\n 擦除测试成功！ " );
 
  else	  
  	printf("\r\n 擦除测试失败！ " );  
}

/*
 * 函数名：SD_SingleBlockTest
 * 描述  ：	单个数据块读写测试
 * 输入  ：无
 * 输出  ：无
 */
void SD_SingleBlockTest(void)
{  
  /* Fill the buffer to send */
  Fill_Buffer(Buffer_Block_Tx, BLOCK_SIZE, 0x320F);

  if (Status == SD_OK)
  {
    /* Write block of 512 bytes on address 0 */
    Status = SD_WriteBlock(Buffer_Block_Tx, 0x00, BLOCK_SIZE);
		
    /* Check if the Transfer is finished */
    Status = SD_WaitWriteOperation();	  
    while(SD_GetStatus() != SD_TRANSFER_OK); 
  }

  if (Status == SD_OK)
  {
    /* Read block of 512 bytes from address 0 */
    Status = SD_ReadBlock(Buffer_Block_Rx, 0x00, BLOCK_SIZE);//读取数据
    /* Check if the Transfer is finished */
    Status = SD_WaitReadOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }

  /* Check the correctness of written data */
  if (Status == SD_OK)
  {
    TransferStatus1 = Buffercmp(Buffer_Block_Tx, Buffer_Block_Rx, BLOCK_SIZE);	//比较
  }
  
  if(TransferStatus1 == PASSED)
    printf("\r\n 单块读写测试成功！" );
 
  else  
  	printf("\r\n 单块读写测试失败！ " );  
}

/*
 * 函数名：SD_MultiBlockTest
 * 描述  ：	多数据块读写测试
 * 输入  ：无
 * 输出  ：无
 */
void SD_MultiBlockTest(void)
{
  /* Fill the buffer to send */
  Fill_Buffer(Buffer_MultiBlock_Tx, MULTI_BUFFER_SIZE, 0x0);

  if (Status == SD_OK)
  {
    /* Write multiple block of many bytes on address 0 */
    Status = SD_WriteMultiBlocks(Buffer_MultiBlock_Tx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS);
    /* Check if the Transfer is finished */
    Status = SD_WaitWriteOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }

  if (Status == SD_OK)
  {
    /* Read block of many bytes from address 0 */
    Status = SD_ReadMultiBlocks(Buffer_MultiBlock_Rx, 0x00, BLOCK_SIZE, NUMBER_OF_BLOCKS);
    /* Check if the Transfer is finished */
    Status = SD_WaitReadOperation();
    while(SD_GetStatus() != SD_TRANSFER_OK);
  }

  /* Check the correctness of written data */
  if (Status == SD_OK)
  {
    TransferStatus2 = Buffercmp(Buffer_MultiBlock_Tx, Buffer_MultiBlock_Rx, MULTI_BUFFER_SIZE);
  }
  
  if(TransferStatus2 == PASSED)	  
  	printf("\r\n 多块读写测试成功！ " );

  else 
  	printf("\r\n 多块读写测试失败！ " );  

}




/*
 * 函数名：Buffercmp
 * 描述  ：比较两个缓冲区中的数据是否相等
 * 输入  ：-pBuffer1, -pBuffer2 : 要比较的缓冲区的指针
 *         -BufferLength 缓冲区长度
 * 输出  ：-PASSED 相等
 *         -FAILED 不等
 */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    if (*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;
}


/*
 * 函数名：Fill_Buffer
 * 描述  ：在缓冲区中填写数据
 * 输入  ：-pBuffer 要填充的缓冲区
 *         -BufferLength 要填充的大小
 *         -Offset 填在缓冲区的第一个值
 * 输出  ：无 
 */
void Fill_Buffer(uint8_t *pBuffer, uint32_t BufferLength, uint32_t Offset)
{
  uint16_t index = 0;

  /* Put in global buffer same values */
  for (index = 0; index < BufferLength; index++ )
  {
    pBuffer[index] = index + Offset;
  }
}

/*
 * 函数名：eBuffercmp
 * 描述  ：检查缓冲区的数据是否为0
 * 输入  ：-pBuffer 要比较的缓冲区
 *         -BufferLength 缓冲区长度        
 * 输出  ：PASSED 缓冲区的数据全为0
 *         FAILED 缓冲区的数据至少有一个不为0 
 */
TestStatus eBuffercmp(uint8_t* pBuffer, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    /* In some SD Cards the erased state is 0xFF, in others it's 0x00 */
    if ((*pBuffer != 0xFF) && (*pBuffer != 0x00))//擦除后是0xff或0x00
    {
      return FAILED;
    }

    pBuffer++;
  }

  return PASSED;
}

/***************************END OF FILE*******************************/
