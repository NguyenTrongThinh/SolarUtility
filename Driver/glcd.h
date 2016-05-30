#ifndef GLCD_H
#define GLCD_H
#ifdef __cplusplus
extern "C"
{
#endif
	#include "KS0108.h"
	#include "graphic.h"
	#include <stm32f10x_gpio.h>
	#define ON			0x01
	#define OFF 		0x00
	void glcd_init(void);
	void glcd_backlinght(unsigned char  onoff);
#ifdef __cplusplus
}
#endif

#endif // GLCD_H
