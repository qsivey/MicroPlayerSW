/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
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


void EXTI9_5_IRQHandler (void)
{
	if (uPlayer.buttonsState == BS_RELEASED)
	{
		if (__HAL_GPIO_EXTI_GET_IT(VOLUME_DOWN_PIN))
			uPlayer.SetEvent(uPL_EVENT_VOLUME_DOWN);

		else if (__HAL_GPIO_EXTI_GET_IT(VOLUME_UP_PIN))
			uPlayer.SetEvent(uPL_EVENT_VOLUME_UP);
	}

	HAL_GPIO_EXTI_IRQHandler(VOLUME_DOWN_PIN);
	HAL_GPIO_EXTI_IRQHandler(VOLUME_UP_PIN);

	uPlayer.buttonsState = BS_PRESSED;
}


void EXTI15_10_IRQHandler (void)
{
	if (uPlayer.buttonsState == BS_RELEASED)
	{
		if (__HAL_GPIO_EXTI_GET_IT(TRACK_BACK_PIN))
			uPlayer.SetEvent(uPL_EVENT_TRACK_BACK);

		else if (__HAL_GPIO_EXTI_GET_IT(TRACK_NEXT_PIN))
			uPlayer.SetEvent(uPL_EVENT_TRACK_NEXT);

		else if (__HAL_GPIO_EXTI_GET_IT(TRACK_PAUSE_PIN))
			uPlayer.SetEvent(uPL_EVENT_TRACK_PAUSE_PLAY);
	}

	HAL_GPIO_EXTI_IRQHandler(TRACK_BACK_PIN);
	HAL_GPIO_EXTI_IRQHandler(TRACK_NEXT_PIN);
	HAL_GPIO_EXTI_IRQHandler(TRACK_PAUSE_PIN);

	uPlayer.buttonsState = BS_PRESSED;
}


void DAC_I2S_TX_DMA_IRQ_HANDLER (void)
{
	uPlayer.DAC_I2S_IRQ_Handler();
}


void ADC_DMA_IRQ_HANDLER (void)
{
	HAL_DMA_IRQHandler(&uPlayer.ADC_DMA_HANDLE);
}


void LCD_SPI_TX_DMA_IRQ_HANDLER (void)
{
	HAL_DMA_IRQHandler(&uPlayer.LCD_SPI_TX_DMA_HANDLE);
}


void HAL_SPI_TxCpltCallback (SPI_HandleTypeDef *hspi)
{
	uPlayer.SPI_TxDMA_TC_Flag = true;
}
