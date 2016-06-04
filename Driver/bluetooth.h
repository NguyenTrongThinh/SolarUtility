#include "stm32f10x.h" 
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
typedef struct
{
	unsigned char Command[100];
	unsigned char Length;
}Command_t;

void Bluetooth_Init(void);
void Bluetooth_Deinit(void);
bool Bluetooth_connect(void);
bool Bluetooth_GetData(unsigned char Data);
bool Bluetooth_ReceiveValidData(Command_t Data);
void Bluetooth_GetCommand(Command_t *data);

