#include <FreeRTOS.h>
#include <task.h>
#include <Queue.h>
#include <semphr.h>
#include <timers.h>
#include <crystalconfig.h>
#include <relay.h>
#include <buzzer.h>



typedef enum { TR_SUCCESS, TR_ERROR} ErrorCode;

ErrorCode systemInit(void);
ErrorCode Application_Run(void);

void RELAY_TASK(void *pvParameters);
void BUZZER_TASK(void *pvParameters);