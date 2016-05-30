#include <relay.h>

#define RELAY_PORT 					GPIOA
#define RELAY_1_PIN    			GPIO_Pin_8
#define RELAY_2_PIN    			GPIO_Pin_12
#define RELAY_3_PIN    			GPIO_Pin_11
#define RELAY_4_PIN    			GPIO_Pin_15

void relay_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_InitStructure.GPIO_Pin   =  RELAY_1_PIN | RELAY_2_PIN | RELAY_3_PIN | RELAY_4_PIN;
	GPIO_InitStructure.GPIO_Speed =  GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  =  GPIO_Mode_Out_PP;
	GPIO_Init(RELAY_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(RELAY_PORT, RELAY_4_PIN);
}
void relay_onoff(unsigned char relayID, unsigned char  onoff)
{
	switch (relayID)
	{
		case RELAY_1:
			if (onoff)
				GPIO_SetBits(RELAY_PORT, RELAY_1_PIN);
			else
				GPIO_ResetBits(RELAY_PORT, RELAY_1_PIN);
			break;
		case RELAY_2:
			if (onoff)
				GPIO_SetBits(RELAY_PORT, RELAY_2_PIN);
			else
				GPIO_ResetBits(RELAY_PORT, RELAY_2_PIN);
			break;
		case RELAY_3:
			if (onoff)
				GPIO_SetBits(RELAY_PORT, RELAY_3_PIN);
			else
				GPIO_ResetBits(RELAY_PORT, RELAY_3_PIN);
			break;
		case RELAY_4:
			if (onoff)
				GPIO_SetBits(RELAY_PORT, RELAY_4_PIN);
			else
				GPIO_ResetBits(RELAY_PORT, RELAY_4_PIN);
			break;
		default:
			break;
	}
}

