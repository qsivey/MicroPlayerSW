/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#include "rtc.h"


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   Class Methods
 */
void qcRealTimeClock::RTC_Init (void)
{
	RTC_PrivilegeStateTypeDef privilegeState = { 0 };
	RTC_TimeTypeDef sTime = { 0 };

	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct1 = {0};

	PeriphClkInitStruct1.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	PeriphClkInitStruct1.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;

	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct1) != HAL_OK)
		HardwareErrorHandler();

	__HAL_RCC_RTC_ENABLE();
	__HAL_RCC_RTCAPB_CLK_ENABLE();

	RTC_HANDLE.Instance = RTC;
	RTC_HANDLE.Init.HourFormat = RTC_HOURFORMAT_24;
	RTC_HANDLE.Init.AsynchPrediv = 97;
	RTC_HANDLE.Init.SynchPrediv = 325;
	RTC_HANDLE.Init.OutPut = RTC_OUTPUT_DISABLE;
	RTC_HANDLE.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
	RTC_HANDLE.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	RTC_HANDLE.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	RTC_HANDLE.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
	RTC_HANDLE.Init.BinMode = RTC_BINARY_NONE;

	if (HAL_RTC_Init(&RTC_HANDLE) != HAL_OK)
		HardwareErrorHandler();

	privilegeState.rtcPrivilegeFull = RTC_PRIVILEGE_FULL_NO;
	privilegeState.backupRegisterPrivZone = RTC_PRIVILEGE_BKUP_ZONE_NONE;
	privilegeState.backupRegisterStartZone2 = RTC_BKP_DR0;
	privilegeState.backupRegisterStartZone3 = RTC_BKP_DR0;

	if (HAL_RTCEx_PrivilegeModeSet(&RTC_HANDLE, &privilegeState) != HAL_OK)
		HardwareErrorHandler();

	sTime.Hours = 0x0;
	sTime.Minutes = 0x0;
	sTime.Seconds = 0x0;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;

	if (HAL_RTC_SetTime(&RTC_HANDLE, &sTime, RTC_FORMAT_BIN) != HAL_OK)
		HardwareErrorHandler();

	__HAL_RCC_CLEAR_RESET_FLAGS();
}


ui32 qcRealTimeClock::GetElapsedTime (void)
{
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    HAL_RTC_GetTime(&RTC_HANDLE, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&RTC_HANDLE, &date, RTC_FORMAT_BIN);

    ui32 now_sec = time.Hours * 3600 + time.Minutes * 60 + time.Seconds;
//    int start_sec = playerTime.startTime.Hours * 3600 + playerTime.startTime.Minutes * 60 + playerTime.startTime.Seconds;
//
//    int elapsed = now_sec - start_sec;
//    if (elapsed < 0)
//    	elapsed += 24 * 3600;

    return now_sec;
}
