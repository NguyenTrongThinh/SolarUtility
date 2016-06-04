#include <xtask.h>

#define SET_RELAY_CMD_CODE 					0x0A
#define SET_RELAY_TON_OFF_CMD_CODE 			0x0B
#define SET_DATE_TIME_CMD_CODE				0x0C
#define SET_FONT_DATA_CMD_CODE				0x0D
#define SET_BUZZER_CMD_CODE					0x0E

#define RELAY_TASK_STACK_SIZE					(configMINIMAL_STACK_SIZE)
#define BUZZER_TASK_STACK_SIZE					(configMINIMAL_STACK_SIZE)
#define BLUETOOTH_TASK_STACK_SIZE				(configMINIMAL_STACK_SIZE)
#define BUTTON_TASK_STACK_SIZE				(configMINIMAL_STACK_SIZE)


#define RELAY_TASK_PRIORITY							(tskIDLE_PRIORITY + 1)
#define BUZZER_TASK_PRIORITY						(tskIDLE_PRIORITY + 1)
#define BLUETOOTH_TASK_PRIORITY						(tskIDLE_PRIORITY + 1)
#define BUTTON_TASK_PRIORITY						(tskIDLE_PRIORITY + 1)

typedef struct
{
	unsigned char BeepTimes;
	unsigned char Volume;
	
}Buzzer_t;

unsigned char RelayState = 0;
SemaphoreHandle_t xSemaphore;


xTaskHandle relay_Handle;
xTaskHandle buzzer_Handle;
xTaskHandle bluetooth_Handle;
xTaskHandle button_Handle;
xQueueHandle RxQueue;
xQueueHandle RelayQueue;
xQueueHandle BuzzerQueue;
ErrorCode systemInit(void)
{
	crystal_Init();
	relay_init();
	Bluetooth_Init();
	Buzzer_Init();
	Buzzer_SetDuty(0);
	Buzzer_Start();
	Button_Init();
	RxQueue = xQueueCreate(100, sizeof(unsigned char));
	RelayQueue = xQueueCreate(4, sizeof(unsigned char));
	BuzzerQueue = xQueueCreate(4, sizeof(Buzzer_t));
	xSemaphore = xSemaphoreCreateMutex();
	if (RxQueue != NULL && RelayQueue != NULL && BuzzerQueue != NULL && xSemaphore != NULL)
		return TR_SUCCESS;
	return TR_ERROR;
}
ErrorCode Application_Run(void)
{
	xTaskCreate(RELAY_TASK, "RELAY", RELAY_TASK_STACK_SIZE, NULL, RELAY_TASK_PRIORITY, &relay_Handle);	
	xTaskCreate(BUZZER_TASK, "BUZZER", BUZZER_TASK_STACK_SIZE, NULL, BUZZER_TASK_PRIORITY, &buzzer_Handle);	
	xTaskCreate(BLUETOOTH_TASK, "BLUETOOTH", BLUETOOTH_TASK_STACK_SIZE, NULL, BLUETOOTH_TASK_PRIORITY, &bluetooth_Handle);	
	xTaskCreate(BUTTON_TASK, "BUTTON", BUTTON_TASK_STACK_SIZE, NULL, BUTTON_TASK_PRIORITY, &button_Handle);	

	vTaskStartScheduler();
	return TR_SUCCESS;
}
void BUTTON_TASK(void *pvParameters)
{
	unsigned char oldValue = 0, value = 0, tmp;
	unsigned char i = 0;
	Buzzer_t BuzzerData;
	while(1)
	{
		value = Button_Read();
		
		if (value != oldValue)
		{  
			BuzzerData.BeepTimes = 1;
			BuzzerData.Volume = 20;
			xQueueSendToBack( BuzzerQueue, ( void * ) &BuzzerData, ( TickType_t ) 10 );	
			if( xSemaphoreTake( xSemaphore, ( TickType_t ) 200 ) == pdTRUE )
			{
				for (i = 0; i < 4; i++)
				{
					tmp = (value >> i) & 0x01;
					if (tmp == 1)
					{
						RelayState ^= (tmp << i);
					}
				}
				xSemaphoreGive( xSemaphore );
			}
			oldValue = value;
			xQueueSendToBack( RelayQueue, ( void * ) &RelayState, ( TickType_t ) 10 );
		}
		vTaskDelay(100);
	}
}
void BLUETOOTH_TASK(void *pvParameters)
{
	portBASE_TYPE xStatus;
	unsigned char ReadValue;
	Command_t ReceiveData;
	unsigned char state = 0;
	Buzzer_t BuzzerData;
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
							  case SET_BUZZER_CMD_CODE:
								  BuzzerData.BeepTimes = ReceiveData.Command[2];
								  BuzzerData.Volume = ReceiveData.Command[3];
								  
								  xQueueSendToBack( BuzzerQueue, ( void * ) &BuzzerData, ( TickType_t ) 10 );
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
				if( xSemaphoreTake( xSemaphore, ( TickType_t ) 200 ) == pdTRUE )
				{
					RelayState = ReadValue;
					xSemaphoreGive( xSemaphore );
				}
			}
		}
	}
}

void BUZZER_TASK(void *pvParameters)
{
	portBASE_TYPE xStatus;
	Buzzer_t ReadValue;
	unsigned char i;
	while(1)
	{
		if (uxQueueMessagesWaiting(BuzzerQueue) != NULL)
		{
			xStatus = xQueueReceive(BuzzerQueue, &ReadValue, 1);
			if (xStatus == pdPASS)
			{
				Buzzer_Start();
				for(i = 0; i < ReadValue.BeepTimes; i++)
				{
					Buzzer_SetDuty(ReadValue.Volume);
					vTaskDelay(100);
					Buzzer_SetDuty(0);
					vTaskDelay(200);
				}
				Buzzer_Stop();
			}
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
