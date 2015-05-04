#include "stm32f10x_gpio.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define CNT_NUM	17

void cnt_task(void* par);

unsigned int cnt_get(int n);

void cnt_reset(int n);
