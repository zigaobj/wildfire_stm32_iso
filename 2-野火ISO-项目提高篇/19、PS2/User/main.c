#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_chipid.h"
#include "delay.h"
#include "ps2.h" 
#include "mouse.h"

 /**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
	uint16_t Buf[10], i;
	
	delay_init(72);
	/* USART1 config 9600 8-N-1 */
	USART1_Config();

	Init_Mouse();
	
	for(;;)
	{
		if(PS2_Status&0x80)
		{
			Mouse_Data_Pro();
			PS2_Status=MOUSE;
			printf("X = %d Y = %d    \r", MouseX.x_pos,  MouseX.y_pos);
			PS2_En_Data_Report();
			
		}
		else if(PS2_Status&0x40)
		{
			PS2_Status=MOUSE;
		}
		
		delay_ms(1);	
		
	}
}
/*********************************************END OF FILE**********************/
