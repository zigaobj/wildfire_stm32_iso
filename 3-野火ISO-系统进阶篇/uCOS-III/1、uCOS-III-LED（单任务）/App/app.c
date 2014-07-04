#include "app.h"

/* 
 * 函数名：Task_LED
 * 描述  : LED流水灯	
 * 输入  ：无
 * 输出  : 无
 */
void Task_LED(void *p_arg)
{
	OS_ERR err;
  (void)p_arg;                		// 'p_arg' 并没有用到，防止编译器提示警告

  while (1)
  {
      LED1( ON );
      OSTimeDlyHMSM(0, 0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
      LED1( OFF);
      
      LED2( ON );
      OSTimeDlyHMSM(0, 0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
      LED2( OFF);
      
      LED3( ON );
      OSTimeDlyHMSM(0, 0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);
      LED3( OFF);
  }
}
