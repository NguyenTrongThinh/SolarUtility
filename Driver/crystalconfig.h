#ifndef CRYTALCONFIG_H
#define CRYTALCONFIG_H
#ifdef __cplusplus
extern "C"
{
#endif
	#include "stm32f10x.h" 
	#include <stm32f10x_rcc.h>
	void crystal_Init(void);
#ifdef __cplusplus
}
#endif

#endif // CRYTALCONFIG_H
