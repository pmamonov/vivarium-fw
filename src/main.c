#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "stm32f10x_gpio.h"
//#include "stdlib.h"
//#include "string.h"
#include "cdcio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "strtok.h"
#include "blink.h"
#include "adc.h"
#include "chat.h"

/*#define ADC_NCHAN 4
volatile int chan[ADC_NCHAN];
xSemaphoreHandle chanLock;*/

void vChatTask(void* vpars);
void vBlinkTask(void* vpars);
void status_err();
void status_ok();

int main(void){
  portBASE_TYPE err;
  char s[64];

//  SystemInit();
  Set_System();

  // USB setup
  Set_USBClock();
  USB_Interrupts_Config();
  USB_Init();


  err = xTaskCreate( vBlinkTask, "blink", 64, NULL, tskIDLE_PRIORITY+1, NULL );
  if ( err == pdPASS)
    cdc_write_buf(&cdc_out, "blink started\n", 0);
  else{
    sniprintf(s, sizeof(s),"blink failed %d", err);
    cdc_write_buf(&cdc_out, s, 0);
  }
/*
  err = xTaskCreate( vChatTask, "chat", 1024, NULL, tskIDLE_PRIORITY+1, NULL );
  if (err == pdPASS)
    cdc_write_buf(&cdc_out, "chat started\n", 0);
  else{
    sniprintf(s, sizeof(s),"chat failed %d", err);
    cdc_write_buf(&cdc_out, s, 0);
  }
*/
  vTaskStartScheduler();

  while(1);
}

void vApplicationStackOverflowHook( xTaskHandle xTask, signed portCHAR *pcTaskName ){
  while(1);
}
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif
