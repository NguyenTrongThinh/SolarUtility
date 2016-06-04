#include <xtask.h>

#define SET_RELAY_CMD_CODE 					0x0A
#define SET_RELAY_TON_OFF_CMD_CODE 			0x0B
#define SET_DATE_TIME_CMD_CODE				0x0C
#define SET_FONT_DATA_CMD_CODE				0x0D
#define SET_BUZZER_CMD_CODE					0x0E

#define RELAY_TASK_STACK_SIZE					(configMINIMAL_STACK_SIZE)
#define BUZZER_TASK_STACK_SIZE					(configMINIMAL_STACK_SIZE)
#define BLUETOOTH_TASK_STACK_SIZE				(configMINIMAL_STACK_SIZE)


#define RELAY_TASK_PRIORITY							(tskIDLE_PRIORITY + 1)
#define BUZZER_TASK_PRIORITY						(tskIDLE_PRIORITY + 1)
#define BLUETOOTH_TASK_PRIORITY						(tskIDLE_PRIORITY + 1)

xTaskHandle relay_Handle;
xTaskHandle buzzer_Handle;
xTaskHandle bluetooth_Handle;
xQueueHandle RxQueue;
xQueueHandle RelayQueue;
ErrorCode systemInit(void)
{
	crystal_Init();
	relay_init();
	Bluetooth_Init();
	Buzzer_Init();
	Buzzer_SetDuty(0);
	Buzzer_Start();
	RxQueue = xQueueCreate(100, sizeof(unsigned char));
	RelayQueue = xQueueCreate(4, sizeof(unsigned char));
	if (RxQueue != NULL && RelayQueue != NULL)
		return TR_SUCCESS;
	return TR_ERROR;
}
ErrorCode Application_Run(void)
{
	xTaskCreate(RELAY_TASK, "RELAY", RELAY_TASK_STACK_SIZE, NULL, RELAY_TASK_PRIORITY, &relay_Handle);	
	xTaskCreate(BUZZER_TASK, "BUZZER", BUZZER_TASK_STACK_SIZE, NULL, BUZZER_TASK_PRIORITY, &buzzer_Handle);	
	xTaskCreate(BLUETOOTH_TASK, "BLUETOOTH", BLUETOOTH_TASK_STACK_SIZE, NULL, BLUETOOTH_TASK_PRIORITY, &bluetooth_Handle);	
	
	vTaskStartScheduler();
	return TR_SUCCESS;
}
void BLUETOOTH_TASK(void *pvParameters)
{
	portBASE_TYPE xStatus;
	unsigned char ReadValue;
	Command_t ReceiveData;
	unsigned char state = 0;
	while(1)
	{
		if (uxQueueMessagesWaiting(RxQueue) != NULL)
		{
			xStatus = xQueueReceive(RxQueue, &ReadValue, 1);
			if (xStatus == pdPASS)
			{
				  if (Bluetooth_GetData(ReadValue))
				  {
					  Bluetooth_GetCommand(&ReceiveData);
					  
					  if (Bluetooth_ReceiveValidData(ReceiveData))
					  {	
						  switch(ReceiveData.Command[1])
						  {
							  case SET_RELAY_CMD_CODE:
								  xQueueSendToBack( RelayQueue, ( void * ) &ReceiveData.Command[2], ( TickType_t ) 10 );
								  break;
						  }
					  }
					  else
					  {
						  printf("Invalid Data\n");
						  for (state = 0; state < ReceiveData.Length; state++)
						  printf("%02x", ReceiveData.Command[state]);
					  }
				  }
			}
		}
	}
}
void RELAY_TASK(void *pvParameters)
{
	portBASE_TYPE xStatus;
	unsigned char tmp, ReadValue, i;
	while(1)
	{
		if (uxQueueMessagesWaiting(RelayQueue) != NULL)
		{
			xStatus = xQueueReceive(RelayQueue, &ReadValue, 1);
			if (xStatus == pdPASS)
			{
				for (i = 0;  i < 4; i++)
				{
					tmp = (ReadValue >> i)&0x01;
					relay_onoff(i + 1, tmp);
				}
			}
		}
	}
}

void BUZZER_TASK(void *pvParameters)
{
	unsigned int i = 0;
	Buzzer_SetDuty(20);
	while(1)
	{
		for (i = 0; i < 100; i++)
		{
			Buzzer_Start();
			vTaskDelay(300);
			Buzzer_Stop();
			vTaskDelay(3000);
		}
	}
}

void USART2_IRQHandler(void)
{
	unsigned char  ReceiveData;
	static portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
      ReceiveData =(unsigned char)USART_ReceiveData(USART2);
      xQueueSendToBackFromISR(RxQueue, &ReceiveData, &xHigherPriorityTaskWoken);
	}
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}
