#include "bsp_can.h"
#include "bsp_led.h"
#include "stdio.h"

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* 在中断处理函数中返回 */
__IO uint32_t ret = 0;

volatile TestStatus TestRx;	

/*
 * 函数名：CAN_NVIC_Configuration
 * 描述  ：CAN RX0 中断优先级配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	/* Enable CAN1 RX0 interrupt IRQ channel */
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;     // 主优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // 次优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
 * 函数名：CAN_GPIO_Config
 * 描述  ：CAN GPIO 和时钟配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：内部调用
 */
static void CAN_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);	                        											 
	
	/* CAN1 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
	/* Configure CAN pin: RX */									               // PB8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	             // 上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Configure CAN pin: TX */									               // PB9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		         // 复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//#define GPIO_Remap_CAN    GPIO_Remap1_CAN1 本实验没有用到重映射I/O
	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);
}

/*
 * 函数名：CAN_Polling
 * 描述  ：配置 CAN 的工作模式为 回环模式
 * 输入  ：无
 * 输出  : -PASSED   成功
 *         -FAILED   失败
 * 调用  ：内部调用
 */
TestStatus CAN_Polling(void)
{
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	CanTxMsg TxMessage;
	CanRxMsg RxMessage;
	uint32_t i = 0;
	uint8_t TransmitMailbox = 0;
	
	/* CAN register init */
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	
	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM=DISABLE;						 // 时间触发通信禁止
	CAN_InitStructure.CAN_ABOM=DISABLE;						 // 离线退出是在中断置位清0后退出
	CAN_InitStructure.CAN_AWUM=DISABLE;						 // 自动唤醒模式：清零sleep
	CAN_InitStructure.CAN_NART=DISABLE;						 // 自动重新传送豹纹，知道发送成功
	CAN_InitStructure.CAN_RFLM=DISABLE;						 // FIFO没有锁定，新报文覆盖旧报文  
	CAN_InitStructure.CAN_TXFP=DISABLE;						 // 发送报文优先级确定：标志符
	CAN_InitStructure.CAN_Mode=CAN_Mode_LoopBack;	 // 回环模式
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;				 // 1tq、BS1、BS2的值跟波特率有关
	CAN_InitStructure.CAN_BS1=CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2=CAN_BS2_7tq;
	CAN_InitStructure.CAN_Prescaler=5;             // 分频系数为5
	CAN_Init(CAN1, &CAN_InitStructure);            // 初始化CAN1 
	
	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber=0;
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	/* transmit */
	TxMessage.StdId=0x11;				// 设定标准标识符（11位，扩展的为29位）
	TxMessage.RTR=CAN_RTR_DATA;	// 传输消息的帧类型为数据帧（还有远程帧）
	TxMessage.IDE=CAN_ID_STD;		// 消息标志符实验标准标识符
	TxMessage.DLC=2;					  // 发送两帧，一帧8位
	TxMessage.Data[0]=0xCA;			// 第一帧数据
	TxMessage.Data[1]=0xFE;			// 第二帧数据
	
	TransmitMailbox=CAN_Transmit(CAN1, &TxMessage);
	i = 0;
	// 用于检查消息传输是否正常
	while((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (i != 0xFF))
	{
		i++;
	}
	
	i = 0;
	// 检查返回的挂号的信息数目
	while((CAN_MessagePending(CAN1, CAN_FIFO0) < 1) && (i != 0xFF))
	{
		i++;
	}
	
	/* receive */
	RxMessage.StdId=0x00;
	RxMessage.IDE=CAN_ID_STD;
	RxMessage.DLC=0;
	RxMessage.Data[0]=0x00;
	RxMessage.Data[1]=0x00;
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	
	
	if (RxMessage.StdId!=0x11)
	{
		return FAILED;  
	}
	
	if (RxMessage.IDE!=CAN_ID_STD)
	{
		return FAILED;
	}
	
	if (RxMessage.DLC!=2)
	{
		return FAILED;  
	}
	
	/* 判断发送的信息和接收的信息是否相等 */
	if ((RxMessage.Data[0]<<8|RxMessage.Data[1])!=0xCAFE)
	{
		return FAILED;
	}
	
	//printf("receive data:0X%X,0X%X",RxMessage.Data[0], RxMessage.Data[1]);  
	
	return PASSED; /* Test Passed */
}

/**
  * @brief  Configures the CAN, transmit and receive using interrupt.
  * @param  None
  * @retval : PASSED if the reception is well done, FAILED in other case
  */

/*
 * 函数名：CAN_Interrupt
 * 描述  ：配置 CAN 的工作模式为 中断模式
 * 输入  ：无
 * 输出  : -PASSED   成功
 *         -FAILED   失败
 * 调用  ：内部调用
 */
TestStatus CAN_Interrupt(void)
{
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	CanTxMsg TxMessage;
	uint32_t i = 0;
	
	/* CAN register init */
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	
	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM=DISABLE;           // 时间触发通信禁止
	CAN_InitStructure.CAN_ABOM=DISABLE;	          // 离线退出是在中断置位清0后退出
	CAN_InitStructure.CAN_AWUM=DISABLE;	          // 自动唤醒模式：清零sleep
	CAN_InitStructure.CAN_NART=DISABLE;	          // 自动重新传送豹纹，知道发送成功
	CAN_InitStructure.CAN_RFLM=DISABLE;	          // FIFO没有锁定，新报文覆盖旧报文
	CAN_InitStructure.CAN_TXFP=DISABLE;           // 发送报文优先级确定：标志符
	CAN_InitStructure.CAN_Mode=CAN_Mode_LoopBack; // 回环模式
	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;        // 1tq、BS1、BS2的值跟波特率有关
	CAN_InitStructure.CAN_BS1=CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2=CAN_BS2_7tq;
	CAN_InitStructure.CAN_Prescaler=1;					  // 分频系数为1
	CAN_Init(CAN1, &CAN_InitStructure);						// 初始化CAN1
	
	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber=1;
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	
	/* CAN FIFO0 message pending interrupt enable */ 
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE); //中断使能
	
	/* transmit 1 message */
	TxMessage.StdId=0x00;					 // 标准标识符为0
	TxMessage.ExtId=0x1234;				 // 设置扩展标示符（29位）
	TxMessage.IDE=CAN_ID_EXT;			 // 使用扩展标识符
	TxMessage.RTR=CAN_RTR_DATA;		 // 消息类型为数据帧，一帧8位
	TxMessage.DLC=2;							 // 发送两帧信息
	TxMessage.Data[0]=0xDE;				 // 第一帧信息
	TxMessage.Data[1]=0xCA;				 // 第二帧信息
	CAN_Transmit(CAN1, &TxMessage);
	
	/* initialize the value that will be returned */
	ret = 0xFF;
	
	/* receive message with interrupt handling */
	i=0;
	while((ret == 0xFF) && (i < 0xFFF))
	{
	i++;
	}
	
	if (i == 0xFFF)
	{
	ret=0;  
	}
	
	/* disable interrupt handling */
	CAN_ITConfig(CAN1, CAN_IT_FMP0, DISABLE);
	
	return (TestStatus)ret;
}

/*
 * 函数名：CAN_Init
 * 描述  ：CAN初始化，包括端口初始化和中断优先级初始化
 * 输入  ：无
 * 输出  : 无	 
 * 调用  ：外部调用
 */
void USER_CAN_Init(void)
{
	CAN_NVIC_Configuration();
	CAN_GPIO_Config();
}

/*
 * 函数名：CAN_Test
 * 描述  ：CAN 回环模式跟中断模式测试
 * 输入  ：无
 * 输出  : 无	 
 * 调用  ：外部调用
 */
void USER_CAN_Test(void)
{
	/* CAN transmit at 100Kb/s and receive by polling in loopback mode */
	TestRx = CAN_Polling();
	
	if (TestRx == FAILED)
	{    
		LED1( OFF );	// LED1 OFF 		
	}
	else
	{    
		LED1( ON );	  // LED1 ON;		
	}
	
	/* CAN transmit at 500Kb/s and receive by interrupt in loopback mode */
	TestRx = CAN_Interrupt();
	
	if (TestRx == FAILED)
	{    
		LED2( OFF );	// LED2 OFF;		 
	}
	else
	{   
		LED2( ON );	  // LED2 ON;		
	}
}


/*-------------------------------------------------------------------------------*/

void CAN_Loopback_Test(void)
{	
	/* LED config */
	LED_GPIO_Config();	
	
	printf( "\r\n 这个一个CAN（回环模式和中断模式）测试程序...... \r\n" );
	
	USER_CAN_Init();	
	printf( "\r\n CAN 回环测试初始化成功...... \r\n" );
	
	USER_CAN_Test();	
	printf( "\r\n CAN 回环测试成功...... \r\n" );	
	
	while (1)
	{
	
	}
}

/*----------------------------------------END OF FILE---------------------------------------------------*/
