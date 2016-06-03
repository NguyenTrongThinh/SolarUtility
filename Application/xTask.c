#include <xtask.h>

#define RELAY_TASK_STACK_SIZE					(configMINIMAL_STACK_SIZE)
#define BUZZER_TASK_STACK_SIZE					(configMINIMAL_STACK_SIZE)

#define RELAY_TASK_PRIORITY							(tskIDLE_PRIORITY + 1)
#define BUZZER_TASK_PRIORITY						(tskIDLE_PRIORITY + 1)

xTaskHandle relay_Handle;
xTaskHandle buzzer_Handle;

ErrorCode systemInit(void)
{
	crystal_Init();
	relay_init();
	Buzzer_Init();
	Buzzer_SetDuty(0);
	Buzzer_Start();
	return TR_SUCCESS;
}
ErrorCode Application_Run(void)
{
	xTaskCreate(RELAY_TASK, "RELAY", RELAY_TASK_STACK_SIZE, NULL, RELAY_TASK_PRIORITY, &relay_Handle);	
	xTaskCreate(BUZZER_TASK, "RELAY", BUZZER_TASK_STACK_SIZE, NULL, BUZZER_TASK_PRIORITY, &buzzer_Handle);	
	
	vTaskStartScheduler();
	return TR_SUCCESS;
}

void RELAY_TASK(void *pvParameters)
{
	while(1)
	{
		relay_onoff(RELAY_1, ON);
		vTaskDelay(1000);
		relay_onoff(RELAY_1, OFF);
		vTaskDelay(1000);
	}
}
void BUZZER_TASK(void *pvParameters)
{
	unsigned int i = 0;
	Buzzer_SetDuty(50);
	while(1)
	{
		for (i = 0; i < 5000; i++)
		{
			Buzzer_SetFreq(i);
			vTaskDelay(10);
		}
	}
}