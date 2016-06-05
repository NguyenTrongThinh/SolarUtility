#include <FreeRTOS.h>
#include <task.h>
#include <Queue.h>
#include <semphr.h>
#include <timers.h>
#include <crystalconfig.h>
#include <relay.h>
#include <buzzer.h>
#include <bluetooth.h>
#include <button.h>
#include <ds1307.h>



typedef enum { TR_SUCCESS, TR_ERROR} ErrorCode;

ErrorCode systemInit(void);
ErrorCode Application_Run(void);

void RELAY_TASK(void *pvParameters);
void BUZZER_TASK(void *pvParameters);
void BLUETOOTH_TASK(void *pvParameters);
void BUTTON_TASK(void *pvParameters);
void EEPROM_RTC_TASK(void *pvParameters);