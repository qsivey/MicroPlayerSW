/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#include	"PCM5142.h"


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   Class Methods
 */
void qcPCM5142::PCM5142_Init (void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	DAC_I2S_DMA_CLOCK_ENABLE();

	HAL_NVIC_SetPriority(DAC_I2S_TX_DMA_CH_IRQN, 0, 0);
	HAL_NVIC_EnableIRQ(DAC_I2S_TX_DMA_CH_IRQN);

	DAC_I2S_HANDLE.Instance = DAC_I2S_INSTANCE;
	DAC_I2S_HANDLE.Init.Mode = I2S_MODE_MASTER_TX;
	DAC_I2S_HANDLE.Init.Standard = I2S_STANDARD_PHILIPS;
	DAC_I2S_HANDLE.Init.DataFormat = I2S_DATAFORMAT_16B;
	DAC_I2S_HANDLE.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
	DAC_I2S_HANDLE.Init.AudioFreq = I2S_AUDIOFREQ_44K;
	DAC_I2S_HANDLE.Init.CPOL = I2S_CPOL_LOW;
	DAC_I2S_HANDLE.Init.FirstBit = I2S_FIRSTBIT_MSB;
	DAC_I2S_HANDLE.Init.WSInversion = I2S_WS_INVERSION_DISABLE;
	DAC_I2S_HANDLE.Init.Data24BitAlignment = I2S_DATA_24BIT_ALIGNMENT_RIGHT;
	DAC_I2S_HANDLE.Init.MasterKeepIOState = I2S_MASTER_KEEP_IO_STATE_DISABLE;

	if (HAL_I2S_Init(&DAC_I2S_HANDLE) != HAL_OK)
		HardwareErrorHandler();

	/* GPIO & Clock Init */
	DAC_I2S_CLOCK_ENABLE();
	DAC_I2S_GPIO_CLOCK_ENABLE();

	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = DAC_I2S_SD_PIN;
	GPIO_InitStruct.Alternate = DAC_I2S_SD_PIN_AF;
	HAL_GPIO_Init(DAC_I2S_SD_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DAC_I2S_CK_PIN;
	GPIO_InitStruct.Alternate = DAC_I2S_CK_PIN_AF;
	HAL_GPIO_Init(DAC_I2S_CK_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DAC_I2S_WS_PIN;
	GPIO_InitStruct.Alternate = DAC_I2S_WS_PIN_AF;
	HAL_GPIO_Init(DAC_I2S_WS_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DAC_I2S_MCK_PIN;
	GPIO_InitStruct.Alternate = DAC_I2S_MCK_PIN_AF;
	HAL_GPIO_Init(DAC_I2S_MCK_GPIO_PORT, &GPIO_InitStruct);

	/* DMA Node Configs */
	auto SetupNode = [](DMA_NodeConfTypeDef &cfg, uint32_t srcWidth, uint32_t destWidth)
	{
		cfg.NodeType = DMA_GPDMA_LINEAR_NODE;
		cfg.Init.Request = DAC_I2S_TX_DMA_REQUEST;
		cfg.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
		cfg.Init.Direction = DMA_MEMORY_TO_PERIPH;
		cfg.Init.SrcInc = DMA_SINC_INCREMENTED;
		cfg.Init.DestInc = DMA_DINC_FIXED;
		cfg.Init.SrcDataWidth = srcWidth;
		cfg.Init.DestDataWidth = destWidth;
		cfg.Init.SrcBurstLength = 1;
		cfg.Init.DestBurstLength = 1;
		cfg.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT1;
		cfg.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
		cfg.Init.Mode = DMA_NORMAL;
		cfg.TriggerConfig.TriggerPolarity = DMA_TRIG_POLARITY_MASKED;
		cfg.DataHandlingConfig.DataExchange = DMA_EXCHANGE_NONE;
		cfg.DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;
	};

	DMA_NodeConfTypeDef NodeConfig16, NodeConfig32;

	SetupNode(NodeConfig16, DMA_SRC_DATAWIDTH_HALFWORD, DMA_DEST_DATAWIDTH_HALFWORD);
	SetupNode(NodeConfig32, DMA_SRC_DATAWIDTH_WORD, DMA_DEST_DATAWIDTH_WORD);

	HAL_DMAEx_List_BuildNode(&NodeConfig16, &DAC_I2S_TX_DMA_NODE_16B);
	HAL_DMAEx_List_InsertNode(&DAC_I2S_TX_DMA_LIST_16B, NULL, &DAC_I2S_TX_DMA_NODE_16B);
	HAL_DMAEx_List_SetCircularMode(&DAC_I2S_TX_DMA_LIST_16B);

	HAL_DMAEx_List_BuildNode(&NodeConfig32, &DAC_I2S_TX_DMA_NODE_32B);
	HAL_DMAEx_List_InsertNode(&DAC_I2S_TX_DMA_LIST_32B, NULL, &DAC_I2S_TX_DMA_NODE_32B);
	HAL_DMAEx_List_SetCircularMode(&DAC_I2S_TX_DMA_LIST_32B);

	DAC_I2S_TX_DMA_HANDLE.Instance = DAC_I2S_TX_DMA_INSTANCE;
	DAC_I2S_TX_DMA_HANDLE.InitLinkedList.Priority = DMA_HIGH_PRIORITY;
	DAC_I2S_TX_DMA_HANDLE.InitLinkedList.LinkStepMode = DMA_LSM_FULL_EXECUTION;
	DAC_I2S_TX_DMA_HANDLE.InitLinkedList.LinkAllocatedPort = DMA_LINK_ALLOCATED_PORT1;
	DAC_I2S_TX_DMA_HANDLE.InitLinkedList.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
	DAC_I2S_TX_DMA_HANDLE.InitLinkedList.LinkedListMode = DMA_LINKEDLIST_CIRCULAR;

	if (HAL_DMAEx_List_Init(&DAC_I2S_TX_DMA_HANDLE) != HAL_OK)
		HardwareErrorHandler();

	__HAL_LINKDMA(&DAC_I2S_HANDLE, hdmatx, DAC_I2S_TX_DMA_HANDLE);

	if (HAL_DMA_ConfigChannelAttributes(&DAC_I2S_TX_DMA_HANDLE, DMA_CHANNEL_NPRIV) != HAL_OK)
		HardwareErrorHandler();

	/* I2C Init */
	DAC_I2C_GPIO_CLOCK_ENABLE();
	DAC_I2C_CLOCK_ENABLE();

	GPIO_InitStruct.Pin = DAC_I2C_SDA_PIN | DAC_I2C_SCL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = DAC_I2C_PINS_AF;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	DAC_I2C_HANDLE.Instance = DAC_I2C_INSTANCE;
	DAC_I2C_HANDLE.Init.Timing = 0x60404E72;
	DAC_I2C_HANDLE.Init.OwnAddress1 = 0;
	DAC_I2C_HANDLE.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	DAC_I2C_HANDLE.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	DAC_I2C_HANDLE.Init.OwnAddress2 = 0;
	DAC_I2C_HANDLE.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	DAC_I2C_HANDLE.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	if (HAL_I2C_Init(&DAC_I2C_HANDLE) != HAL_OK)
		HardwareErrorHandler();

	/* Other Init */
	GPIO_InitStruct.Pin = DAC_XSMT_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DAC_XSMT_GPIO_PORT, &GPIO_InitStruct);

	/* Page 0 */
	PCM5142_Write(0, 0);

	/* Reset */
	PCM5142_Write(2, 0x10);  // Standby
	PCM5142_Write(1, 0x11);  // Reset registers

	qmDelayMs(20);

	PCM5142_Write(2, 0);  // Normal operation

	qmDelayMs(20);

	PCM5142_Write(3, 0);  // Digital unmute
	PCM5142_Write(4, 1);  // Enable PLL

//	PCM5142_Write(9, 0);  // I2S Input
//	PCM5142_Write(14, 0);  // DAC clock source
//	PCM5142_Write(37, 0);  // Regard all detections
//	PCM5142_Write(40, 0);	// I2S Data Format : I2S + I2S Word Length :
							//	00: 16 bits
							//	01: 20 bits
							//	10: 24 bits
							//	11: 32 bits
//	PCM5142_Write(42, 0x11);  // Left/Right DAC Data Path : Left channel data + Right channel data
//	PCM5142_Write(43, 1);  // DSP Program Selection : 8x/4x/2x FIR interpolation filter with de-emphasis
//	PCM5142_Write(121, 0);  // DAC Architecture

	/* Other Init */
	volume = qcfgINITIAL_VOLUME;
	amplitudeBoost = false;

	currentSampleRate = 0;
	currentBitRate = 0;

	/* Start */
	PCM5142_SetVolume(volume);
	PCM5142_Unmute();
}


void qcPCM5142::PCM5142_Write (ui8 reg, ui8 value)
{
	if (HAL_I2C_Mem_Write(&DAC_I2C_HANDLE, PCM5142_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, qcfgHAL_TIMEOUT_DELAY) != HAL_OK)
		HardwareErrorHandler();
}


void qcPCM5142::PCM5142_Read (ui8 reg, ui8 *value)
{
	if (HAL_I2C_Mem_Read(&DAC_I2C_HANDLE, PCM5142_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, value, 1, qcfgHAL_TIMEOUT_DELAY) != HAL_OK)
		HardwareErrorHandler();
}


void qcPCM5142::PCM5142_Mute (void)
{
	HAL_GPIO_WritePin(DAC_XSMT_GPIO_PORT, DAC_XSMT_PIN, GPIO_PIN_RESET);
}


void qcPCM5142::PCM5142_Unmute (void)
{
	HAL_GPIO_WritePin(DAC_XSMT_GPIO_PORT, DAC_XSMT_PIN, GPIO_PIN_SET);
}


/* 0 --> 16 : Mute -> +103 dB */
void qcPCM5142::PCM5142_SetVolume (ui8 volume)
{
	if (!volume)
	{
		PCM5142_Write(61, 0xFF);  // Digital volume L
		PCM5142_Write(62, 0xFF);  // Digital volume R
	}

	else
	{
		PCM5142_Write(61, 256 - (volume * 13));  // Digital volume L
		PCM5142_Write(62, 256 - (volume * 13));  // Digital volume R
	}
}


void qcPCM5142::PCM5142_SetBitRate (ui8 bitRate)
{
	if (currentBitRate == bitRate)
		return;

	HAL_I2S_DeInit(&DAC_I2S_HANDLE);
	__HAL_DMA_DISABLE(&DAC_I2S_TX_DMA_HANDLE);

	if (HAL_DMAEx_List_Init(&DAC_I2S_TX_DMA_HANDLE) != HAL_OK)
		HardwareErrorHandler();

	switch (bitRate)
	{
		case 16:
			if (HAL_DMAEx_List_LinkQ(&DAC_I2S_TX_DMA_HANDLE, &DAC_I2S_TX_DMA_LIST_16B) != HAL_OK)
				HardwareErrorHandler();

			__HAL_LINKDMA(&DAC_I2S_HANDLE, hdmatx, DAC_I2S_TX_DMA_HANDLE);

			DAC_I2S_HANDLE.Init.DataFormat = I2S_DATAFORMAT_16B;

			if (HAL_I2S_Init(&DAC_I2S_HANDLE) != HAL_OK)
				HardwareErrorHandler();

			PCM5142_Write(40, 0);  // 16 bit

			break;

		case 24:
			if (currentBitRate != 32)
			{
				if (HAL_DMAEx_List_LinkQ(&DAC_I2S_TX_DMA_HANDLE, &DAC_I2S_TX_DMA_LIST_32B) != HAL_OK)
					HardwareErrorHandler();

				__HAL_LINKDMA(&DAC_I2S_HANDLE, hdmatx, DAC_I2S_TX_DMA_HANDLE);

				DAC_I2S_HANDLE.Init.DataFormat = I2S_DATAFORMAT_32B;

				if (HAL_I2S_Init(&DAC_I2S_HANDLE) != HAL_OK)
					HardwareErrorHandler();
			}

			PCM5142_Write(40, 2);  // 24 bit

			break;

		case 32:
			if (currentBitRate != 24)
			{
				if (HAL_DMAEx_List_LinkQ(&DAC_I2S_TX_DMA_HANDLE, &DAC_I2S_TX_DMA_LIST_32B) != HAL_OK)
					HardwareErrorHandler();

				__HAL_LINKDMA(&DAC_I2S_HANDLE, hdmatx, DAC_I2S_TX_DMA_HANDLE);

				DAC_I2S_HANDLE.Init.DataFormat = I2S_DATAFORMAT_32B;

				if (HAL_I2S_Init(&DAC_I2S_HANDLE) != HAL_OK)
					HardwareErrorHandler();
			}

			PCM5142_Write(40, 3);  // 32 bit

			break;

		default:
			return;
	}

	currentBitRate = bitRate;
}


void qcPCM5142::PCM5142_SetSampleRate (ui32 sampleRate)
{
	if (currentSampleRate == sampleRate)
		return;

	switch (sampleRate)
	{
		case 32000 :
		{
			DAC_I2S_HANDLE.Init.AudioFreq = I2S_AUDIOFREQ_32K;

			break;
		}

		case 44100 :
		{
//			PeriphClkInitStruct.PLL2.PLL2N = 65;
//			PeriphClkInitStruct.PLL2.PLL2P = 4;

			DAC_I2S_HANDLE.Init.AudioFreq = I2S_AUDIOFREQ_44K;

			break;
		}

		case 48000 :
		{
			DAC_I2S_HANDLE.Init.AudioFreq = I2S_AUDIOFREQ_48K;

			break;
		}

		case 96000 :
		{
			DAC_I2S_HANDLE.Init.AudioFreq = I2S_AUDIOFREQ_96K;

			break;
		}

		case 192000 :
		{
			DAC_I2S_HANDLE.Init.AudioFreq = I2S_AUDIOFREQ_192K;

			break;
		}

		case 384000 :
		{
			DAC_I2S_HANDLE.Init.AudioFreq = I2S_AUDIOFREQ_192K * 2;

			break;
		}

		default :
			return;  // Not supported sample rate
	}

//	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
//		HardwareErrorHandler();

	if (HAL_I2S_Init(&DAC_I2S_HANDLE) != HAL_OK)
		HardwareErrorHandler();

	currentSampleRate = sampleRate;
}
