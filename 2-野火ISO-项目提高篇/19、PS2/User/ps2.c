/*
*********************************************************************************************************
*	                                  
*	模块名称 : 延时函数	 
*	文件名称 : bsp_ps2.c
*	版    本 : V1.0
*	说    明 : 鼠标驱动           
*	修改记录 : 这里的修改记录也是指的原子开发板的修改记录
*			   
*
*   版本号     日期       作者      说明
*   v1.0     2012-10-26  Eric2013  ST固件库版本 V3.5.0版本。		
*   BLOG: http://blog.sina.com.cn/u/2565749395
*********************************************************************************************************
*/
#include "ps2.h" 
#include "delay.h"

/* PS2产生的时钟频率在10~20Khz(最大33K) */
/* 高/低电平的持续时间为25~50us之间.    */	  			 
/* PS2_Status当前状态标志  */
/* [7]:接收到一次数据;[6]:校验错误;[5:4]:当前工作的模式;[3:0]:收到的数据长度;  */

u8 PS2_Status=CMDMODE; /* 默认为命令模式 */ 
u8 PS2_DATA_BUF[16];   /* ps2数据缓存区  */

/* 位计数器 */
u8 BIT_Count=0;

/*
*********************************************************************************************************
*	函 数 名: PS2_IRQHandler 
*	功能说明: 中断15~10处理函数
* 			  每11个bit,为接收1个字节
* 			  每接收完一个包(11位)后,设备至少会等待50ms再发送下一个包
* 			  只做了鼠标部分,键盘部分暂时未加入	 
*	形    参：无			  
*	返 回 值: 无
*********************************************************************************************************
*/	   
void PS2_IRQHandler(void)
{	  	 
	static u8 tempdata=0;            /* 用于存储临时数据 */
	static u8 parity=0;				 /* 用于奇偶校验     */
	
	/* 中断13产生了相应的中断 */  	   
	if(EXTI->PR&(1<<4))
	{
		EXTI->PR=1<<4;               /* 清除LINE11上的中断标志位 */

		if(BIT_Count==0)
		{
			parity=0;
			tempdata=0;
		}
		BIT_Count++;    
		if(BIT_Count>1&&BIT_Count<10)/* 这里获得数据 */
		{	  
			tempdata>>=1;		     /* 从低位到高位 */
			if(PS2_SDA)
			{
				tempdata|=0x80;
				parity++;			 /* 记录1的个数,用于奇偶校验 */
			}   
		}
		else if(BIT_Count==10)		 /* 得到校验位 */
		{
			if(PS2_SDA)parity|=0x80; /* 校验位为1  */
		}	   
		if(BIT_Count==11)			 /* 接收到1个字节的数据了 */
		{
 			BIT_Count=parity&0x7f;	 /* 取得1的个数 */
			/* 检测奇偶校验OK */	  
			if(((BIT_Count%2==0)&&(parity&0x80))||((BIT_Count%2==1)&&(parity&0x80)==0))
			{					    
				//PS2_Status|=1<<7;				 /* 标记得到数据 */	   
				BIT_Count=PS2_Status&0x0f;		
				PS2_DATA_BUF[BIT_Count]=tempdata;/* 保存数据    */
				if(BIT_Count<15)PS2_Status++;    /* 数据长度加1 */
				BIT_Count=PS2_Status&0x30;	     /* 得到模式    */	  
				switch(BIT_Count)
				{	
				    /* 命令模式下,每收到一个字节都会产生接收完成 */
					case CMDMODE:
						PS2_Dis_Data_Report();	/* 禁止数据传输 */
						PS2_Status|=1<<7; 		/* 标记得到数据 */
						break;

					case KEYBOARD:
						break;

					case MOUSE:
						/* 标准鼠标,3个字节 */
						if(MOUSE_ID==0)		   
						{
							if((PS2_Status&0x0f)==3)
							{
								PS2_Status|=1<<7;	  /* 标记得到数据 */
								PS2_Dis_Data_Report();/* 禁止数据传输 */
							}
						}
						/* 扩展鼠标,4个字节 */
						else if(MOUSE_ID==3)
						{
							if((PS2_Status&0x0f)==4)
							{
								PS2_Status|=1<<7;	  /* 标记得到数据 */
								PS2_Dis_Data_Report();/* 禁止数据传输 */
							}
						}	 
						break;
				}		   		 
			}
			/* 奇偶校验错误 */
			else
			{
				PS2_Status|=1<<6; /* 标记校验错误 */
				PS2_Status&=0xf0; /* 清除接收数据计数器 */
			}
			BIT_Count=0;
		} 	 	  
	}			  
}

/*
*********************************************************************************************************
*	函 数 名: PS2_Dis_Data_Report 
*	功能说明: 禁止数据传输
*			  把时钟线拉低,禁止数据传输
*	形    参：无			  
*	返 回 值: 无
*********************************************************************************************************
*/
void PS2_Dis_Data_Report(void)
{
	PS2_Set_Int(0);    /* 关闭中断      */
	PS2_SET_SCL_OUT(); /* 设置SCL为输出 */
	PS2_SCL_OUT=0;     /* 抑制传输      */
}

/*
*********************************************************************************************************
*	函 数 名: PS2_En_Data_Report 
*	功能说明: 使能数据传输
*             释放时钟线
*	形    参：无			  
*	返 回 值: 无
*********************************************************************************************************
*/
void PS2_En_Data_Report(void)
{
	PS2_SET_SCL_IN();  /* 设置SCL为输入 */
	PS2_SET_SDA_IN();  /* SDA IN        */
	PS2_SCL_OUT=1;     /* 上拉          */   
	PS2_SDA_OUT=1; 
	PS2_Set_Int(1);    /* 开启中断      */
}

/*
*********************************************************************************************************
*	函 数 名: PS2_Set_Int 
*	功能说明: PS2中断屏蔽设置
*	形    参：en:1，开启;0，关闭;			  
*	返 回 值: 无
*********************************************************************************************************
*/
void PS2_Set_Int(u8 en)
{
	/* 清除LINE11上的中断标志位 */
	EXTI->PR=1<<4;  

	if(en)
	{
		EXTI->IMR|=1<<4;		/* 不屏蔽line13上的中断 */
	}
    else 
	{
		EXTI->IMR&=~(1<<4);    /* 屏蔽line13上的中断 */ 
	}

}

/*
*********************************************************************************************************
*	函 数 名: Wait_PS2_Scl 
*	功能说明: 等待PS2时钟线sta状态改变
*	形    参：sta:  1 表示等待变为1;
*					0 表示待变为0;			  
*	返 回 值: 0 表示时钟线变成了sta;
*             1 表示超时溢出;
*********************************************************************************************************
*/
u8 Wait_PS2_Scl(u8 sta)
{
	u16 t=0;
	sta=!sta;
	while(PS2_SCL==sta)
	{
		delay_us(1);
		t++;
		if(t>16000)return 1;/* 时间溢出 (设备会在10ms内检测这个状态) */
	}
	return 0;               /* 被拉低了 */
}

/*
*********************************************************************************************************
*	函 数 名: PS2_Get_Byte 
*	功能说明: 在发送命令/数据之后,等待设备应答,该函数用来获取应答
*	形    参：无			  
*	返 回 值: 返回0，且PS2_Status.6=1，则产生了错误
*********************************************************************************************************
*/
u8 PS2_Get_Byte(void)
{
	u16 t=0;
	u8 temp=0;
	while(1)					/* 最大等待55ms */
	{
		t++;
		delay_us(10);
		if(PS2_Status&0x80)		/* 得到了一次数据 */
		{
			temp=PS2_DATA_BUF[PS2_Status&0x0f-1];
			PS2_Status&=0x70;	/* 清除计数器，接收到数据标记 */
			break;	
		}else if(t>5500||PS2_Status&0x40)break;/* 超时溢出/接收错误 */	   
	}
	PS2_En_Data_Report();		/* 使能数据传输 */
	return temp;    
}

/*
*********************************************************************************************************
*	函 数 名: PS2_Send_Cmd 
*	功能说明: 发送一个命令到PS2.
*	形    参：cmd 命令			  
*	返 回 值: 返回值:0，无错误,其他,错误代码
*********************************************************************************************************
*/	    
u8 PS2_Send_Cmd(u8 cmd)
{
	u8 i;
	u8 high=0;				/* 记录1的个数    */		 
	PS2_Set_Int(0);   		/* 屏蔽中断		  */
	PS2_SET_SCL_OUT();		/* 设置SCL为输出  */
	PS2_SET_SDA_OUT();		/* SDA OUT		  */
	PS2_SCL_OUT=0;			/* 拉低时钟线	  */
	delay_us(120);			/* 保持至少100us  */
	PS2_SDA_OUT=0;			/* 拉低数据线	  */
	delay_us(10);
	PS2_SET_SCL_IN();		/* 释放时钟线,这里PS2设备得到第一个位,开始位  */
	PS2_SCL_OUT=1;
	if(Wait_PS2_Scl(0)==0)	/* 等待时钟拉低  */
	{									  
		for(i=0;i<8;i++)
		{
			if(cmd&0x01)
			{
			    PS2_SDA_OUT=1;
				high++;
			}else PS2_SDA_OUT=0;   
			cmd>>=1;
			/* 这些地方没有检测错误,因为这些地方不会产生死循环 */
			Wait_PS2_Scl(1);		    /* 等待时钟拉高 发送8个位 */
			Wait_PS2_Scl(0);			/* 等待时钟拉低       */
		}
		if((high%2)==0)PS2_SDA_OUT=1;   /* 发送校验位 10      */
		else PS2_SDA_OUT=0;
		Wait_PS2_Scl(1); 				/* 等待时钟拉高 10位  */
		Wait_PS2_Scl(0); 				/* 等待时钟拉低       */
		PS2_SDA_OUT=1;   				/* 发送停止位  11     */	  
 		Wait_PS2_Scl(1);				/* 等待时钟拉高 11位  */
		PS2_SET_SDA_IN();				/* SDA in             */
		Wait_PS2_Scl(0);				/* 等待时钟拉低  	  */
		if(PS2_SDA==0)Wait_PS2_Scl(1);	/* 等待时钟拉高 12位  */ 
		else 
		{
			PS2_En_Data_Report();
			return 1;					/* 发送失败 */
		}		
	}else 
	{
		PS2_En_Data_Report();
		return 2;						/* 发送失败 */
	}
	PS2_En_Data_Report();
	return 0;    						/* 发送成功 */
}

/*
*********************************************************************************************************
*	函 数 名: PS2_Init 
*	功能说明: PS2初始化	端口和时钟线的中断 两根线上面都加入了上拉电阻
*             PS2/CLK--------_PC13
*             PS2/DATA--------PG8
*	形    参：无			  
*	返 回 值: 无
*********************************************************************************************************
*/				   
void PS2_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* 初始化时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 

	/* PS2的两个接口都初始化为上拉输入模式 */
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
 					  	  	 
	/* 配置外部中断 */
   	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource4);	
	
	/* 初始化PS2 CLK线上升沿触发 */  	 
	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
  	EXTI_Init(&EXTI_InitStructure);	

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;	    //使能按键所在的外部中断通道
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
} 
