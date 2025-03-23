/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey
 *	Telegram:	@qsivey
 *	Email:		qsivey@gmail.com
 *	____________________________________________________________________
 */

#include	"microPlayer.h"


qc_uPlayer uPlayer;


char UserPath [4];
extern Diskio_drvTypeDef UserDriver [];

FATFS fs;


void AudioPlayInit (void)
{
	/* Common */

	FATFS_LinkDriver(UserDriver, UserPath);
	if (f_mount(&fs, (TCHAR const*)UserPath, 0) != FR_OK);

	uPlayer.DAC_Init();

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);



	/* WAV test */

//	drwav wav;
//
//	drwav_init_file(&wav, "home.wav", NULL);
//
//	size_t framesRead = drwav_read_pcm_frames_s16(&wav, PCM_BUFFER_SIZE / 2, PCM_Buffer);
//
//	HAL_I2S_Transmit_DMA(&DAC_I2S_NAME, (ui16*)PCM_Buffer, PCM_BUFFER_SIZE);
//
//	CS43L22_SetMute(AUDIO_MUTE_OFF);
//
//	while (framesRead)
//	{
//		if (bufferOffset == BUFFER_OFFSET_FULL)
//		{
//			framesRead = drwav_read_pcm_frames_s16(&wav, PCM_BUFFER_SIZE / 4, PCM_Buffer);
//
//			bufferOffset = BUFFER_OFFSET_NONE;
//		}
//
//		if (bufferOffset == BUFFER_OFFSET_HALF)
//		{
//			framesRead = drwav_read_pcm_frames_s16(&wav, PCM_BUFFER_SIZE / 4, &PCM_Buffer[PCM_BUFFER_SIZE / 2]);
//
//			bufferOffset = BUFFER_OFFSET_NONE;
//		}
//	}


	/* FLAC test */

//	drflac* pFlac = drflac_open_file("sweet.flac", NULL);
//
//	if (pFlac == NULL)
//		HardwareErrorHandler();
//
//	size_t framesRead = drflac_read_pcm_frames_s16(pFlac, PCM_BUFFER_SIZE / 2, PCM_Buffer);
//
//	/* Unmute */
//	CS43L22_SetMute(AUDIO_MUTE_OFF);
//
//	HAL_I2S_Transmit_DMA(&DAC_I2S_NAME, (ui16*)PCM_Buffer, PCM_BUFFER_SIZE);
//
//	/* Decode the whole file */
//	while (framesRead)
//	{
//		if (bufferOffset == BUFFER_OFFSET_FULL)
//		{
//			framesRead = drflac_read_pcm_frames_s16(pFlac, PCM_BUFFER_SIZE / 4, PCM_Buffer);
//
//			bufferOffset = BUFFER_OFFSET_NONE;
//		}
//
//		if (bufferOffset == BUFFER_OFFSET_HALF)
//		{
//			framesRead = drflac_read_pcm_frames_s16(pFlac, PCM_BUFFER_SIZE / 4, &PCM_Buffer[PCM_BUFFER_SIZE / 2]);
//
//			bufferOffset = BUFFER_OFFSET_NONE;
//		}
//	}
//
//	drflac_close(pFlac);

}


void SystemClock_Config (void);
void PeriphCommonClock_Config(void);
void Error_Handler (void);


int main(void)
{
	HAL_Init();

	SystemClock_Config();
	PeriphCommonClock_Config();


	AudioPlayInit();

	uPlayer.Play();

	while (1);
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_PLLI2S|RCC_PERIPHCLK_CLK48
                              |RCC_PERIPHCLK_I2S_APB1;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 192;
  PeriphClkInitStruct.PLLI2S.PLLI2SM = 8;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 4;
  PeriphClkInitStruct.PLLI2S.PLLI2SQ = 8;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLI2SQ;
  PeriphClkInitStruct.PLLI2SSelection = RCC_PLLI2SCLKSOURCE_PLLSRC;
  PeriphClkInitStruct.I2sApb1ClockSelection = RCC_I2SAPB1CLKSOURCE_PLLI2S;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void Error_Handler (void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
