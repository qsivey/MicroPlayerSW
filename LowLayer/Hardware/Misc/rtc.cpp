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
#include "st7789.h"

/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   Class Methods
 */
RTC_HandleTypeDef hrtc;

void RealTimeClock::RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_PrivilegeStateTypeDef privilegeState = {0};
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  hrtc.Init.BinMode = RTC_BINARY_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
	  HardwareErrorHandler();
  }
  privilegeState.rtcPrivilegeFull = RTC_PRIVILEGE_FULL_NO;
  privilegeState.backupRegisterPrivZone = RTC_PRIVILEGE_BKUP_ZONE_NONE;
  privilegeState.backupRegisterStartZone2 = RTC_BKP_DR0;
  privilegeState.backupRegisterStartZone3 = RTC_BKP_DR0;
  if (HAL_RTCEx_PrivilegeModeSet(&hrtc, &privilegeState) != HAL_OK)
  {
	  HardwareErrorHandler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
	  HardwareErrorHandler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x18;
  sDate.Year = 0x25;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
	  HardwareErrorHandler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */
  __HAL_RCC_CLEAR_RESET_FLAGS();
}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
    	HardwareErrorHandler();
    }

    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
    __HAL_RCC_RTCAPB_CLK_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

//void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
//{
//
//  if(rtcHandle->Instance==RTC)
//  {
//  /* USER CODE BEGIN RTC_MspDeInit 0 */
//
//  /* USER CODE END RTC_MspDeInit 0 */
//    /* Peripheral clock disable */
//    __HAL_RCC_RTC_DISABLE();
//    __HAL_RCC_RTCAPB_CLK_DISABLE();
//  /* USER CODE BEGIN RTC_MspDeInit 1 */
//
//  /* USER CODE END RTC_MspDeInit 1 */
//  }
//}

void RealTimeClock::StartTimer(char* str)
{
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);

	//char str[16];
//	snprintf(str, sizeof(str), "%02d:%02d", time.Seconds,  time.Minutes);
//	char str2[16];
	snprintf(str, sizeof(str), "%2d-%2d", date.Year, date.Month);

//	ST7789_WriteString(10, 30, str, &Font20, WHITE, BLACK);
//	ST7789_WriteString(10, 60, str2, &Font20, WHITE, BLACK);
    //HAL_RTC_GetTime(&hrtc, &playerTime.startTime, RTC_FORMAT_BIN);
    //playerTime.totalDurationSec = totalSeconds;
}


ui32 RealTimeClock::GetElapsedTime(void)
{
    RTC_TimeTypeDef now;
    HAL_RTC_GetTime(&hrtc, &now, RTC_FORMAT_BIN);

    int now_sec = now.Hours * 3600 + now.Minutes * 60 + now.Seconds;
    int start_sec = playerTime.startTime.Hours * 3600 + playerTime.startTime.Minutes * 60 + playerTime.startTime.Seconds;

    int elapsed = now_sec - start_sec;
    if (elapsed < 0) elapsed += 24 * 3600; // защита на переход через 00:00

    return (ui32)now_sec; //elapsed
}

