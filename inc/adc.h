#ifndef __adc_h
#define __adc_h

#include "stm32f10x_gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

void adc_init(void);
void adc_get(int i);
void vADCTask(void* vpars);

#endif
