#include "stm32f10x.h"
#include "bsp_sdio_sdcard.h"
#include "bsp_usart1.h"	
#include "ff.h"

FIL fnew;													/* file objects */
FATFS fs;													/* Work area (file system object) for logical drives */
FRESULT res; 
UINT br, bw;            					/* File R/W count */
BYTE buffer[4096]={0};       		  /* file copy buffer */
BYTE textFileBuffer[] = "Welcome to use Wildfire iso stm32 Development Board today is a good day";

int main(void)
{
   /* USART1 config */
		USART1_Config();
		printf("\r\n this is a fatfs test demo \r\n");
	
		/* Sdio Interrupt Config */
		NVIC_Configuration();
	
		/* Register work area for each volume (Always succeeds regardless of disk status) */
		f_mount(0,&fs);
	
//		f_mkdir("sub");
//		f_mkdir("sub/sub1");

		/* function disk_initialize() has been called in f_open */
	
		/* Create new file on the drive 0 */
		res = f_open(&fnew, "0:newfile.txt", FA_CREATE_ALWAYS | FA_WRITE );
		 
		if ( res == FR_OK )
		{
			res = f_write(&fnew, textFileBuffer, sizeof(textFileBuffer), &bw);
			f_close(&fnew);      
		}

		res = f_open(&fnew, "0:newfile.txt", FA_OPEN_EXISTING | FA_READ); 	 
		res = f_read(&fnew, buffer, sizeof(buffer), &br); 

		printf("\r\n %s ", buffer);
		
		/* Close open files */
		f_close(&fnew);	                                      
		 
    /* Unregister work area prior to discard it */
    f_mount(0, NULL);
		
		while(1);
}

	  

