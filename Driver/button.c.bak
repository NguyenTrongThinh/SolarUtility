#include <button.h>

#define BUTTON_PORT 		GPIOA
#define BUTTON_1_Pin        GPIO_Pin_6
#define BUTTON_2_Pin        GPIO_Pin_5
#define BUTTON_3_Pin        GPIO_Pin_4
#define BUTTON_4_Pin        GPIO_Pin_1

void Button_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin   =  BUTTON_1_Pin | BUTTON_2_Pin | BUTTON_3_Pin | BUTTON_4_Pin;
	GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  =  GPIO_Mode_IPU;
	GPIO_Init(BUTTON_PORT, &GPIO_InitStructure);
}
unsigned char Button_Read(void)
{
	unsigned char tmp = 0;
	if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_1_Pin) == 0)
		tmp |= 0x01;
	if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_2_Pin) == 0)
		tmp |= 0x02;
	if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_3_Pin) == 0)
		tmp |= 0x04;
	if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_4_Pin) == 0)
		tmp |= 0x08;
	return tmp;
}
