#include <glcd.h>

#define KS0108_BL_PORT 		GPIOA
#define KS0108_BL    			GPIO_Pin_7

void glcd_init(void)
{
	GLCD_Initialize();
	GLCD_ClearScreen();
	
}
void glcd_backlinght(unsigned char  onoff)
{
	if (onoff)
		GPIO_SetBits(KS0108_BL_PORT, KS0108_BL);
	else
		GPIO_ResetBits(KS0108_BL_PORT, KS0108_BL);
}
