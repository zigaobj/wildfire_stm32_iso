/**
  ******************************************************************************
  * @file    bsp_iso_test.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ISO 整板测试程序 bsp
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "bsp_iso_test.h"
#include "stm32f10x.h"
#include "bsp_ili9341_lcd.h"
#include "bsp_usart1.h"
#include "bsp_i2c_ee.h"
#include "bsp_touch.h"
#include "bsp_led.h" 
#include "bsp_key.h" 
#include "bsp_spi_flash.h"
#include "bsp_dht11.h"
#include "bsp_ds18b20.h"
#include "bsp_ov7725.h"
#include "bsp_adc.h"
#include "bsp_breathing.h" 
#include "bsp_rda5807m.h"
#include "enc28j60_test.h"
#include "bsp_can.h"
#include "bsp_rtc.h"
#include "bsp_fsmc_sram.h"
#include "bsp_fsmc_nor.h"
#include "bsp_nand.h"
#include "bsp_UsbMassStorage.h"

extern volatile uint8_t rec_cmd;

/*
 * 系统软件复位
 */
void Soft_Reset(void)
{
  __set_FAULTMASK(1);   /* 关闭所有中断 */  
  NVIC_SystemReset();   /* 系统复位 */
}

/* 
 * ISO 整板测试菜单 
 */
void Test_Menu(void)
{
	printf("\r\n[~~~~~~~~~~~~野火 ISO STM32 整板测试程序~~~~~~~~~~~~~~~]\r\n");
	printf("[------------请选择操作命，命令必须是小写字母----------]\r\n");

	printf("1 - 流水灯\r\n");
	printf("2 - 按键\r\n");
	printf("3 - EEPROM\r\n");
	printf("4 - FLASH\r\n");
  printf("5 - 液晶显示英文\r\n");
  printf("6 - 液晶触摸\r\n");
  printf("7 - RTC(万年历)\r\n");
  printf("8 - ADC\r\n");
//  printf("9 - DAC\r\n");
  printf("a - FM收音机\r\n");
//	printf("b - MP3-helix解码\r\n");
  printf("c - DHT11温湿度\r\n");
  printf("d - DS18b20温度传感器\r\n");
  printf("e - 摄像头(火眼ov7725)\r\n");
  printf("f - CAN(Loopback)\r\n");
  printf("g - 呼吸灯\r\n");
  printf("h - 以太网ENC28J60\r\n");
  printf("i - SRAM(IS62WV51216BLL-55TLI)\r\n");
  printf("j - NORFLASH(S29GL128P10TFI010)\r\n");
  printf("k - NANDFLASH(HY27UF081G2A)\r\n");
  printf("l - Usb-Mass-Storage 测试\r\n");
  
  printf("请输入命令，字母命令必须小写\r\n");
  printf("\r\n");
}

/*
 * 根据串口不同的命令执行不同的程序
 * 用户也可以自行添加相应的程序
 */
void Board_Test(void)
{  
  while( 1 )
  {    
    switch( rec_cmd )
    {
      case '1':
        printf("按回车返回菜单 \r\n");
        printf("实验现象:LED1 LE2 LED3 轮流闪烁 \r\n");
        LED_Test();         
        break;
      
      case '2':
        printf("按回车返回菜单 \r\n");
        printf("实验现象:KEY1控制LED1反转,KEY2控制LE2反转 \r\n");
        Key_Test();        
        break;
      
      case '3':
        printf("按回车返回菜单 \r\n");
        printf("实验现象:串口打印出:eeprom读写测试成功 \r\n");
        Eeprom_Test();
        break;
      
      case '4':
        printf("按回车返回菜单 \r\n");
        printf("实验现象:串口打印出:2M串行flash(W25Q16)测试成功!\r\n");
        SpiFlash_Test();      
        break;
      
      case '5':
        printf("按回车返回菜单 \r\n");
        printf("实验现象:液晶上显示英文字符和变化的数字\r\n");
        LCD_Test();             
        break;

      case '6':
        printf("按回车返回菜单 \r\n");
        printf("实验现象:液晶出现十字交叉，用画笔点击十字交叉进行触摸屏校正\r\n         校正后可实现画笔功能\r\n");
        Touch_Test();         
        break;
      
      case '7':
        printf("按回车返回菜单 \r\n");
        printf("实验步骤 \r\n");
        printf("1-按照提示在串口中输入时间\r\n");
        printf("实验现象:串口输出新历农历和时间戳\r\n");
        RTC_Test();
        break;
      
      case '8':
        printf("按回车返回菜单 \r\n");
        printf("实验现象:串口每隔一段时间打印出采集到的电压值 \r\n         旋转ISO上的电位器，可使用电压值在0~3.25之间变动\r\n");
        ADC_Test();      
        break;
      
//      case '9':
//        printf("按回车返回菜单 \r\n");
//      
//        break;
      
      case 'a':
        printf("按回车返回菜单 \r\n");
        printf("实验步骤 \r\n");
        printf("1-将ISO上的MP3-FM-SPK开关拨到FM档位\r\n");
        printf("2-接上耳机，同时耳机也是天线，当搜不到台的时候可移动耳机线接收信号\r\n");
        printf("实验现象:KEY1向上自动搜台，搜到台LED1亮。KEY2向下自动搜台，搜到台LED2亮 \r\n");
        RDA5807_FM_Test();
        break;
      
//      case 'b': 
//        printf("按回车返回菜单 \r\n");

//        break;

      case 'c':
        printf("按回车返回菜单 \r\n");
        printf("实验步骤 \r\n");
        printf("1-在ISO上插上DHT11模块 \r\n");
        printf("实验现象:串口打印出:湿度为xxx%RH,温度为xxx℃ \r\n");
        DHT11_Test();          
        break;
      
      case 'd': 
        printf("按回车返回菜单 \r\n");
        printf("实验步骤 \r\n");
        printf("1-在ISO上插上DS18b20模块 \r\n");
        printf("实验现象:串口每隔1s打印一次温度值，同时led1闪烁一次\r\n");
        DS18B20_Test();         
        break;
      
      case 'e':
        printf("按回车返回菜单 \r\n");
        printf("实验步骤 \r\n");
        printf("1-在ISO上插上摄像头模块(火眼OV7725)\r\n");
        printf("实验现象:液晶实时显示摄像头拍回来的图像\r\n");
        Camera_Test();        
        break; 
      
      case 'f':
        printf("按回车返回菜单 \r\n");
        printf("实验现象:串口打印信息:CAN回环测试成功同时LED1和LED2亮\r\n");
        CAN_Loopback_Test();      
        break;
      
      case 'g':
        printf("按回车返回菜单 \r\n");
        printf("实验现象:LED1在PWM信号的控制下在呼吸\r\n");
        Breathing_Test();        
        break;
      
      case 'h':
        printf("按回车返回菜单 \r\n");
        printf("实验步骤 \r\n");
        printf("1-在ISO上插上ENC28J60以太网模块，以太网模块用网线连接电脑或者路由器\r\n");
        printf("2-在DOS模式下输入命令: ping 192.168.1.15 \r\n");
        printf("3-电脑的IP地址或者局域网内的电脑的IP地址必须是192.168.1.x\r\n  但不能是192.168.1.15 \r\n");
        ENC28J60_Test();      
        break;
      
      case 'i':
        printf("按回车返回菜单 \r\n");
        printf("实验步骤\r\n");
        printf("1-在ISO上插上SRAM模块\r\n");
        printf("实验现象:串口打印出SRAM测试成功，同时LED2闪烁\r\n");
        SRAM_Test();      
        break;
      
      case 'j':
        printf("按回车返回菜单 \r\n");
        printf("实验步骤\r\n");
        printf("1-在ISO上插上NORFLASH模块\r\n");
        printf("实验现象:串口打印出NORFLASH测试成功，同时LED2闪烁\r\n");
        printf("\r\n 擦除时间有点长，请耐心等待...... \r\n");
        NORFLASH_Test();      
        break;
      
      case 'k':
        printf("按回车返回菜单 \r\n");
        printf("实验步骤\r\n");
        printf("1-在ISO上插上NANDFLASH模块\r\n");
        printf("实验现象:串口打印砃ANDFLASH Test finished,同时LE1闪烁r\n");
        NANDFLASH_Test();      
        break;
      
      case 'l': 
        printf("按回车返回菜单 \r\n");
        printf("实验步骤\r\n");
        printf("1-在ISO上插上SD卡，容量需小于32GB\r\n");
        printf("2-用microusb线连接电脑和ISO上的供电的microusb口(并不是 usb to uart) \r\n");
        printf("实验现象:串口会打印出:野火 ISO STM32 USB MASS STORAGE 实验\r\n         LED1闪烁，同时SD卡会在电脑上模拟出一个U盘\r\n");
        UsbMassStorage_Test();
        break;      
                        
      default:       
        break;
    }
  }
}


/* 
 * ISO 整板测试 
 */
void ISO_Test(void)
{  
  /* 液晶初始化先于串口，不然会有bug，至于是什么原因我也不知道:( */
  LCD_Init();
  
  USART1_Config();

  Test_Menu();
  
  Board_Test();

	while( 1 ){}
}
/* ---------------------------------------end of file------------------------------------- */
