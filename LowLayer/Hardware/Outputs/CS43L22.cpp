/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#include	"CS43L22.h"


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   Class Methods
 */
void qcCS43L22::DAC_Init (void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };

	DAC_I2S_CLOCK_ENABLE();
	DAC_I2S_GPIO_CLOCK_ENABLE();

	GPIO_InitStruct.Pin = DAC_I2S_MCK_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = DAC_I2S_MCK_PIN_AF;
	HAL_GPIO_Init(DAC_I2S_MCK_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DAC_I2S_WS_PIN | DAC_I2S_CK_PIN | DAC_I2S_SD_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = DAC_I2S_SD_PIN_AF;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_PLLI2S | RCC_PERIPHCLK_I2S_APB1;
	PeriphClkInitStruct.PLLI2S.PLLI2SN = 206;  // 206 - 44 kHz | 249 - 48 kHz
	PeriphClkInitStruct.PLLI2S.PLLI2SM = 8;
	PeriphClkInitStruct.PLLI2S.PLLI2SR = 4;
	PeriphClkInitStruct.PLLI2SSelection = RCC_PLLI2SCLKSOURCE_PLLSRC;
	PeriphClkInitStruct.I2sApb1ClockSelection = RCC_I2SAPB1CLKSOURCE_PLLI2S;

	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
		HardwareErrorHandler();

	/* I2S2 DMA Init */
	DAC_I2S_DMA_CLOCK_ENABLE();

	DAC_I2S_TX_DMA_HANDLE.Instance = DAC_I2S_TX_DMA_STREAM;
	DAC_I2S_TX_DMA_HANDLE.Init.Channel = DAC_I2S_TX_DMA_CHANNEL;
	DAC_I2S_TX_DMA_HANDLE.Init.Direction = DMA_MEMORY_TO_PERIPH;
	DAC_I2S_TX_DMA_HANDLE.Init.PeriphInc = DMA_PINC_DISABLE;
	DAC_I2S_TX_DMA_HANDLE.Init.MemInc = DMA_MINC_ENABLE;
	DAC_I2S_TX_DMA_HANDLE.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	DAC_I2S_TX_DMA_HANDLE.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	DAC_I2S_TX_DMA_HANDLE.Init.Mode = DMA_CIRCULAR;
	DAC_I2S_TX_DMA_HANDLE.Init.Priority = DMA_PRIORITY_HIGH;
	DAC_I2S_TX_DMA_HANDLE.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
	DAC_I2S_TX_DMA_HANDLE.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
	DAC_I2S_TX_DMA_HANDLE.Init.MemBurst = DMA_MBURST_INC8;
	DAC_I2S_TX_DMA_HANDLE.Init.PeriphBurst = DMA_PBURST_SINGLE;

	if (HAL_DMA_Init(&DAC_I2S_TX_DMA_HANDLE) != HAL_OK)
		HardwareErrorHandler();

	__HAL_LINKDMA(&DAC_I2S_HANDLE, hdmatx, DAC_I2S_TX_DMA_HANDLE);

	/* I2S2 Init */
	DAC_I2S_HANDLE.Instance = SPI2;
	DAC_I2S_HANDLE.Init.Mode = I2S_MODE_MASTER_TX;
	DAC_I2S_HANDLE.Init.Standard = I2S_STANDARD_PHILLIPS;
	DAC_I2S_HANDLE.Init.DataFormat = I2S_DATAFORMAT_16B;
	DAC_I2S_HANDLE.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
	DAC_I2S_HANDLE.Init.AudioFreq = I2S_AUDIOFREQ_44K;
	DAC_I2S_HANDLE.Init.CPOL = I2S_CPOL_LOW;
	DAC_I2S_HANDLE.Init.ClockSource = I2S_CLOCK_PLL;
	DAC_I2S_HANDLE.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;

	if (HAL_I2S_Init(&DAC_I2S_HANDLE) != HAL_OK)
		HardwareErrorHandler();

	HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

	/* I2C Init */
	DAC_I2C_GPIO_CLOCK_ENABLE();
	DAC_I2C_CLOCK_ENABLE();

	GPIO_InitStruct.Pin = DAC_I2C_SDA_PIN | DAC_I2C_SCL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	DAC_I2C_HANDLE.Instance = DAC_I2C_INSTANCE;
	DAC_I2C_HANDLE.Init.ClockSpeed = 400000;
	DAC_I2C_HANDLE.Init.DutyCycle = I2C_DUTYCYCLE_2;
	DAC_I2C_HANDLE.Init.OwnAddress1 = 0;
	DAC_I2C_HANDLE.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	DAC_I2C_HANDLE.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	DAC_I2C_HANDLE.Init.OwnAddress2 = 0;
	DAC_I2C_HANDLE.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	DAC_I2C_HANDLE.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	if (HAL_I2C_Init(&DAC_I2C_HANDLE) != HAL_OK)
		HardwareErrorHandler();

	/* Other Init */
	GPIO_InitStruct.Pin = DAC_RESET_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DAC_RESET_GPIO_PORT, &GPIO_InitStruct);

	qmDelayMs(50);

	/* CS43L22 reset */
	DAC_Reset();

	/* CS43L22 initialization */

	/* Power off */
	DAC_Write(CS43L22_REG_POWER_CTRL1, 0x01);

	/* Choose headphone */
	DAC_Write(CS43L22_REG_POWER_CTRL2, 0xAF);

	/* Clocks auto */
	DAC_Write(CS43L22_REG_CLOCK_CTRL, 0x81);

	/* Interface set */
	DAC_Write(CS43L22_REG_INTERFACE_CTRL1, 0x07);

	/* Set volume level */
	DAC_SetVolume(qcfgINITIAL_VOLUME);
	volume = qcfgINITIAL_VOLUME;

	/* Disable the analog soft ramp */
	DAC_Write(CS43L22_REG_ANALOG_ZC_SR_SETT, 0x00);

	/* Disable the digital soft ramp */
	DAC_Write(CS43L22_REG_MISC_CTL, 0x04);

	/* Disable the limiter attack level */
	DAC_Write(CS43L22_REG_LIMIT_CTL1, 0x00);

	/* Adjust Bass and Treble levels */
	DAC_Write(CS43L22_REG_TONE_CTL, 0x0F);

	/* Adjust PCM volume level */
	DAC_Write(CS43L22_REG_PCMA_VOL, 0x0A);
	DAC_Write(CS43L22_REG_PCMB_VOL, 0x0A);

	qmDelayMs(10);

	/* Enable the digital soft ramp */
	DAC_Write(CS43L22_REG_MISC_CTL, 0x06);  // 0x06

	/* Mute */
	DAC_SetMute(AUDIO_MUTE_ON);

	/* Power on the Codec */
	DAC_Write(CS43L22_REG_POWER_CTL1, 0x9E);

	currentSampleRate = qcfgINITIAL_SAMPLE_RATE;
}


void qcCS43L22::DAC_Reset (void)
{
	HAL_GPIO_WritePin(DAC_RESET_GPIO_PORT, DAC_RESET_PIN, GPIO_PIN_RESET);
	qmDelayMs(1);

	HAL_GPIO_WritePin(DAC_RESET_GPIO_PORT, DAC_RESET_PIN, GPIO_PIN_SET);
	qmDelayMs(2);
}


void qcCS43L22::DAC_Write (ui8 reg, ui8 value)
{
	HAL_I2C_Mem_Write(&DAC_I2C_HANDLE, qcfgCS43L22_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, qcfgHAL_TIMEOUT_DELAY);
}


void qcCS43L22::DAC_SetMute (ui32 cmd)
{
	/* Set the Mute mode */
	if (cmd == AUDIO_MUTE_ON)
	{
		DAC_Write(CS43L22_REG_POWER_CTL2, 0xFF);
		DAC_Write(CS43L22_REG_HEADPHONE_A_VOL, 0x01);
		DAC_Write(CS43L22_REG_HEADPHONE_B_VOL, 0x01);
	}

	else /* AUDIO_MUTE_OFF Disable the Mute */
	{
		DAC_Write(CS43L22_REG_HEADPHONE_A_VOL, 0x00);
		DAC_Write(CS43L22_REG_HEADPHONE_B_VOL, 0x00);
		DAC_Write(CS43L22_REG_POWER_CTL2, 0xAF);  // HF
	}
}


void qcCS43L22::DAC_SetVolume (ui8 volume)
{
	ui8 convertedVolume = DAC_ConvertVolume(volume);

	if (convertedVolume > 0xE6)
	{
		/* Set the Master volume */
		DAC_Write(CS43L22_REG_MASTER_A_VOL, convertedVolume - 0xE7);
		DAC_Write(CS43L22_REG_MASTER_B_VOL, convertedVolume - 0xE7);
	}

	else
	{
		/* Set the Master volume */
		DAC_Write(CS43L22_REG_MASTER_A_VOL, convertedVolume + 0x19);
		DAC_Write(CS43L22_REG_MASTER_B_VOL, convertedVolume + 0x19);
	}
}


void qcCS43L22::DAC_SetSampleRate (ui32 sampleRate)
{
	if (currentSampleRate == sampleRate)
		return;

	switch (sampleRate)
	{
		case 44100 :
		{
			RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };

			PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_PLLI2S | RCC_PERIPHCLK_I2S_APB1;
			PeriphClkInitStruct.PLLI2S.PLLI2SN = 206;  // 206 - 44 kHz | 249 - 48 kHz
			PeriphClkInitStruct.PLLI2S.PLLI2SM = 8;
			PeriphClkInitStruct.PLLI2S.PLLI2SR = 4;
			PeriphClkInitStruct.PLLI2SSelection = RCC_PLLI2SCLKSOURCE_PLLSRC;
			PeriphClkInitStruct.I2sApb1ClockSelection = RCC_I2SAPB1CLKSOURCE_PLLI2S;

			if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
				HardwareErrorHandler();

			DAC_I2S_HANDLE.Init.AudioFreq = I2S_AUDIOFREQ_44K;

			if (HAL_I2S_Init(&DAC_I2S_HANDLE) != HAL_OK)
				HardwareErrorHandler();

			currentSampleRate = 44100;

			break;
		}

		case 48000 :
		{
			RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };

			PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_PLLI2S | RCC_PERIPHCLK_I2S_APB1;
			PeriphClkInitStruct.PLLI2S.PLLI2SN = 249;  // 206 - 44 kHz | 249 - 48 kHz
			PeriphClkInitStruct.PLLI2S.PLLI2SM = 8;
			PeriphClkInitStruct.PLLI2S.PLLI2SR = 4;
			PeriphClkInitStruct.PLLI2SSelection = RCC_PLLI2SCLKSOURCE_PLLSRC;
			PeriphClkInitStruct.I2sApb1ClockSelection = RCC_I2SAPB1CLKSOURCE_PLLI2S;

			if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
				HardwareErrorHandler();

			DAC_I2S_HANDLE.Init.AudioFreq = I2S_AUDIOFREQ_48K;

			if (HAL_I2S_Init(&DAC_I2S_HANDLE) != HAL_OK)
				HardwareErrorHandler();

			break;
		}

		default :
			return;  // Not supported sample rate
	}

	currentSampleRate = sampleRate;
}


void qcCS43L22::DAC_I2S_IRQ_Handler (void)
{
	/* Half */
	if (__HAL_DMA_GET_FLAG(&DAC_I2S_TX_DMA_HANDLE, DMA_FLAG_HTIF0_4))
	{
		__HAL_DMA_CLEAR_FLAG(&DAC_I2S_TX_DMA_HANDLE, DMA_FLAG_HTIF0_4);
		bufferOffset = BUFFER_OFFSET_FULL;
	}

	/* Full */
	else if (__HAL_DMA_GET_FLAG(&DAC_I2S_TX_DMA_HANDLE, DMA_FLAG_TCIF0_4))
	{
		__HAL_DMA_CLEAR_FLAG(&DAC_I2S_TX_DMA_HANDLE, DMA_FLAG_TCIF0_4);
		bufferOffset = BUFFER_OFFSET_HALF;
	}
}
