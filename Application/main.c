#include <crystalconfig.h>
#include <glcd.h>
#include <relay.h>
#include <AMES_Delay.h>
#include <glcd.h>

#define SUCCESS							0x01
#define ERROR								0x00


int main(void)
{
	int i;
	crystal_Init();
	glcd_init();
	glcd_backlinght(ON);
	relay_init();
	while(1)
	{
		for (i = 1; i < 5; i++)
		{
			relay_onoff(i, ON);
			Delay_ms(1000);
		}
		for (i = 1; i < 5; i++)
		{
			relay_onoff(i, OFF);
			Delay_ms(1000);
		}
	}
	return SUCCESS;
}

