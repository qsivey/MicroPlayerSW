/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#ifndef		LOW_LAYER_INITIAL_CONFIG_H_
#define		LOW_LAYER_INITIAL_CONFIG_H_

#ifdef 		__cplusplus
extern 		"C" {
#endif
/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

#include	"adjunct.h"


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *												  Initial Project Config
 */
#define		PROJ_USE_FREERTOS		setOFF

#if (PROJ_USE_FREERTOS == setOFF)
	#define	qmGetTick()				HAL_GetTick()
	#define	qmDelayMs(ms)			HAL_Delay(ms)

	#define	qmMalloc(size)			malloc(size)
	#define	qmFree(addr)			free(addr)
#endif


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *															Service Code
 */
/* -------------------------------------------------
 * MCU depended libraries
 */
#include	"stm32f4xx_hal.h"

/* -------------------------------------------------
 * OS depended libraries
 */
#if (PROJ_USE_FREERTOS)
	/* OS includes */
	#include "FreeRTOS.h"
	#include "task.h"
	#include "croutine.h"
	#include "semphr.h"
	#include "queue.h"
	#include "timers.h"

	/* OS priorities */
	#define	OS_LOW_PRIORITY			(0)
	#define	OS_NORMAL_PRIORITY		(configMAX_PRIORITIES - 4)
	#define	OS_MEDIUM_PRIORITY		(configMAX_PRIORITIES - 3)
	#define	OS_HIGH_PRIORITY		(configMAX_PRIORITIES - 2)
	#define	OS_HIGHEST_PRIORITY		(configMAX_PRIORITIES - 1)

	/* OS objects determination */
	#define	osTaskCreate			xTaskCreate
	#define	osTaskDelete(pH)		vTaskDelete(pH)

	#define	osTaskStartSheduler()	vTaskStartScheduler()

	#define	osSystemLock()			vPortEnterCritical()
	#define	osSystemUnlock()		vPortExitCritical()

	#define	osMalloc(size)			pvPortMalloc(size)
	#define	osFree(p)				vPortFree(p)

	#define	osTaskYIELD()			taskYIELD()

	#define	osDelay(t)				vTaskDelay(t)
	#define	osGetTickCount()		xTaskGetTickCount()

	#define	osMaxDelay				portMAX_DELAY

	#define	osSemaphoreCreate(smphr)		vSemaphoreCreateBinary(smphr)
	#define osSemaphoreTake(smphr, bTime)	xSemaphoreTake(smphr, bTime)
	#define osSemaphoreGive(smphr)			xSemaphoreGive(smphr)

	/* Gluing with main shell */
	#define	qmGetTick()				osGetTickCount()
	#define	qmDelayMs(ms)			osDelay(ms)

	#define	qmMalloc(size)			osMalloc(size)
	#define	qmFree(addr)			osFree(addr)
#endif

#define		qmCreate(type)			(type*)qmMalloc(sizeof(type))
#define		qmCreateArr(type, size)	(type*)qmMalloc(size)
#define		qmDestroy(addr)			qmFree(addr); addr = NULL

#define		qmCustomDelay(x)		for (ui32 i = 0; i < x; i++) __NOP();


/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */
#ifdef 		__cplusplus
			}
#endif

#endif		/* LOW_LAYER_INITIAL_CONFIG_H_ */
