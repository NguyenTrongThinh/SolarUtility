#include "stm32f10x.h" 
#include <stm32f10x_gpio.h>
#include <stm32f10x_i2c.h>

void __I2C_Init(void);
void EEPROM_Write_Byte(unsigned int Address, unsigned char Byte);
void EEPROM_Write(unsigned int Address, unsigned char *Data, unsigned int Length);
unsigned char EEPROM_Read_Byte(unsigned char Address);
void EEPROM_Read(unsigned char Address, unsigned char *Data, unsigned int Length);

