#include "stm32f10x.h" 
#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include <stdlib.h>
#include <stdio.h>

#define BUZZER_CHANNEL_PORT				GPIOA
#define BUZZER_CHANNEL_1_PIN 			GPIO_Pin_0		

void Buzzer_Init(void);
void Buzzer_SetDuty(unsigned int percent);
void Buzzer_SetFreq(unsigned long freq);
void Buzzer_Start(void);
void Buzzer_Stop(void);
