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


#if (qcfgPROJ_USE_FREERTOS == setOFF)

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


/* Buttons */
void VOLUME_DOWN_IRQ_HANDLER (void)
{
	if (uPlayer.Buttons[BUT_VOL_DOWN].status == BS_RELEASED)
	{
		uPlayer.Buttons[BUT_VOL_DOWN].status = BS_ANTI_BOUNCE;
		uPlayer.Buttons[BUT_VOL_DOWN].pressTime = qmGetTick();
	}

	HAL_GPIO_EXTI_IRQHandler(uPlayer.Buttons[BUT_VOL_DOWN].pin);
}


void VOLUME_UP_IRQ_HANDLER (void)
{
	if (uPlayer.Buttons[BUT_VOL_UP].status == BS_RELEASED)
	{
		uPlayer.Buttons[BUT_VOL_UP].status = BS_ANTI_BOUNCE;
		uPlayer.Buttons[BUT_VOL_UP].pressTime = qmGetTick();
	}

	HAL_GPIO_EXTI_IRQHandler(uPlayer.Buttons[BUT_VOL_UP].pin);
}


void TRACK_PLAY_IRQ_HANDLER (void)
{
	if (uPlayer.Buttons[BUT_TRACK_PLAY].status == BS_RELEASED)
	{
		uPlayer.Buttons[BUT_TRACK_PLAY].status = BS_ANTI_BOUNCE;
		uPlayer.Buttons[BUT_TRACK_PLAY].pressTime = qmGetTick();
	}

	HAL_GPIO_EXTI_IRQHandler(uPlayer.Buttons[BUT_TRACK_PLAY].pin);
}


void TRACK_BACK_IRQ_HANDLER (void)
{
	if (uPlayer.Buttons[BUT_TRACK_BACK].status == BS_RELEASED)
	{
		uPlayer.Buttons[BUT_TRACK_BACK].status = BS_ANTI_BOUNCE;
		uPlayer.Buttons[BUT_TRACK_BACK].pressTime = qmGetTick();
	}

	HAL_GPIO_EXTI_IRQHandler(uPlayer.Buttons[BUT_TRACK_BACK].pin);
}


void TRACK_NEXT_IRQ_HANDLER (void)
{
	if (uPlayer.Buttons[BUT_TRACK_NEXT].status == BS_RELEASED)
	{
		uPlayer.Buttons[BUT_TRACK_NEXT].status = BS_ANTI_BOUNCE;
		uPlayer.Buttons[BUT_TRACK_NEXT].pressTime = qmGetTick();
	}

	HAL_GPIO_EXTI_IRQHandler(uPlayer.Buttons[BUT_TRACK_NEXT].pin);
}


/* SDMMC1 */
extern SD_HandleTypeDef hsd1;

void SDMMC1_IRQHandler (void)
{
	HAL_SD_IRQHandler(&hsd1);
}


/* I2S1 */
void DAC_I2S_TX_DMA_IRQ_HANDLER (void)
{
	HAL_DMA_IRQHandler(&uPlayer.DAC_I2S_TX_DMA_HANDLE);
}


void HAL_I2S_TxHalfCpltCallback (I2S_HandleTypeDef *hi2s)
{
	uPlayer.PCM5142_I2S_TxHalfCpltCallback();
}


void HAL_I2S_TxCpltCallback (I2S_HandleTypeDef *hi2s)
{
	uPlayer.PCM5142_I2S_TxHalfCallback();
}


/* SPI3 */
void SPI3_IRQHandler (void)
{
	HAL_SPI_IRQHandler(&uPlayer.LCD_SPI_HANDLE);
}


void LCD_SPI_TX_DMA_IRQ_HANDLER (void)
{
	HAL_DMA_IRQHandler(&uPlayer.LCD_SPI_TX_DMA_HANDLE);
}


void HAL_SPI_TxCpltCallback (SPI_HandleTypeDef *hspi)
{
	uPlayer.SPI_TxDMA_EOT = true;
}
