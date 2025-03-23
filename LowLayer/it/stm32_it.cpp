/** ____________________________________________________________________
 *
 * 	@file		stm32f4xx_it.cpp
 *
 *	@author		qsivey
 *
 *	VK / TG:	@qsivey
 *	mail:		qsivey@gmail.com
 *
 *	K-Lab		Laboratory of robotics and mechatronics
 *	____________________________________________________________________
 */

#include "stm32_it.h"


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *											   Common Interrupt Handlers
 */
void NMI_Handler (void)
{
	while (1);
}


void HardFault_Handler (void)
{
	while (1);
}


void MemManage_Handler (void)
{
	while (1);
}


void BusFault_Handler (void)
{
	while (1);
}


void UsageFault_Handler (void)
{
	while (1);
}


void DebugMon_Handler (void)
{
	;
}


#if (PROJ_USE_FREERTOS == setOFF)

	void SVC_Handler (void)
	{
		;
	}


	void PendSV_Handler (void)
	{
		;
	}


	void SysTick_Handler (void)
	{
		HAL_IncTick();
	}

#endif


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *										   Peripheral Interrupt Handlers
 */
extern qc_uPlayer uPlayer;

static ui8 pause = 0;
ui8 dirFlag = 1;  // 0 - back : 2 - next

ui32 buttonTime = 0;


void EXTI9_5_IRQHandler (void)
{
	if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_9))
	{
		if ((qmGetTick() - buttonTime) > 500)
		{
			uPlayer.VolumeDown();

			buttonTime = qmGetTick();
		}

		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
	}

	else if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_8))
	{
		if ((qmGetTick() - buttonTime) > 500)
		{
			uPlayer.VolumeUp();

			buttonTime = qmGetTick();
		}

		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
	}
}


void EXTI15_10_IRQHandler (void)
{
	if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_14))
	{
		if ((qmGetTick() - buttonTime) > 500)
		{
			dirFlag = 0;

			buttonTime = qmGetTick();
		}

		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
	}

	else if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13))
	{
		if ((qmGetTick() - buttonTime) > 500)
		{
			dirFlag = 2;

			buttonTime = qmGetTick();
		}

		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
	}

	else if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_15))
	{
		if ((qmGetTick() - buttonTime) > 500)
		{
			if (pause)
			{
				uPlayer.DAC_SetMute(AUDIO_MUTE_OFF);
				uPlayer.DAC_Write(CS43L22_REG_POWER_CTL1, 0x9E);

				HAL_I2S_DMAResume(&uPlayer.DAC_I2S_HANDLE);

				pause = 0;
			}

			else
			{
				uPlayer.DAC_SetMute(AUDIO_MUTE_ON);
				uPlayer.DAC_Write(CS43L22_REG_POWER_CTL1, 0x01);

				HAL_I2S_DMAPause(&uPlayer.DAC_I2S_HANDLE);

				pause = 1;
			}

			buttonTime = qmGetTick();
		}

		HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
	}
}



void DAC_I2S_TX_DMA_IRQ_HANDLER (void)
{
	/* Half */
	if (__HAL_DMA_GET_FLAG(&uPlayer.DAC_I2S_TX_DMA_HANDLE, DMA_FLAG_HTIF0_4))
	{
		__HAL_DMA_CLEAR_FLAG(&uPlayer.DAC_I2S_TX_DMA_HANDLE, DMA_FLAG_HTIF0_4);
		uPlayer.bufferOffset = BUFFER_OFFSET_FULL;
	}

	/* Full */
	else if (__HAL_DMA_GET_FLAG(&uPlayer.DAC_I2S_TX_DMA_HANDLE, DMA_FLAG_TCIF0_4))
	{
		__HAL_DMA_CLEAR_FLAG(&uPlayer.DAC_I2S_TX_DMA_HANDLE, DMA_FLAG_TCIF0_4);
		uPlayer.bufferOffset = BUFFER_OFFSET_HALF;
	}
}
