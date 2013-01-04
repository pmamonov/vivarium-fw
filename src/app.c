/**
  ******************************************************************************
  * @file    app.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file provides all the Application firmware functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/ 

#include "usbd_cdc_core.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"
#include <stdio.h>
//#include "newlib_stubs.h"
#include "FreeRTOS.h"
#include "task.h"

#include "stm32f4xx_gpio.h"

void vReadTask(void* vpars);
void vWriteTask(void* vpars);

__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;


int main(void)
{
  __IO uint32_t i = 0;  

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  GPIO_InitTypeDef sGPIOinit;
  sGPIOinit.GPIO_Pin = 1<<15;
  sGPIOinit.GPIO_Mode = GPIO_Mode_OUT;
  sGPIOinit.GPIO_Speed = GPIO_Speed_25MHz;
  sGPIOinit.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOD, &sGPIOinit);

  USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS 
            USB_OTG_HS_CORE_ID,
#else            
            USB_OTG_FS_CORE_ID,
#endif  
            &USR_desc, 
            &USBD_CDC_cb, 
            &USR_cb);

// disable stdio buffering
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

  /* Main loop */
  iprintf("Creating tasks\n");
  xTaskCreate( vReadTask, "echo", 1024, NULL, tskIDLE_PRIORITY+1, NULL );
  xTaskCreate( vWriteTask, "echo", 1024, NULL, tskIDLE_PRIORITY+1, NULL );
  iprintf("Ready\n");
  vTaskStartScheduler();
/*  while (1)
  {
		iprintf("> %s", fgets(b, 100, stdin));
		fflush(stdout);
  }*/
} 

volatile uint8_t echo_buff[100];
volatile buff_ready=0;

void vReadTask(void* vpars){
  iprintf("Reader is here\n");
  while (1)
  {
    if (!buff_ready){
      fgets(echo_buff, 100, stdin);
      GPIO_SetBits(GPIOD, 1<<15);
      buff_ready=1;
    }
  }
}

void vWriteTask(void* vpars){
  iprintf("Writer is here\n");
  while (1)
  {
    if (buff_ready){
		  iprintf(">> %s", echo_buff);
		  fflush(stdout);
      buff_ready=0;
      GPIO_ResetBits(GPIOD, 1<<15);
    }
  }
}

#ifdef USE_FULL_ASSERT
/**
* @brief  assert_failed
*         Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  File: pointer to the source file name
* @param  Line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}
#endif

