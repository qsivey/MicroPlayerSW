/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#include "digitalChannels.h"
#include "projectConfig.h"


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   Class Methods
 */
void qcDigitalChannels::ButtonsInit (void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	DIGITAL_GPIO_CLOCK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	buttonsState = BS_RELEASED;
	buttonsTime = 0;
}


void qcDigitalChannels::ButtonsHandle (void)
{
	if (buttonsState == BS_PRESSED)
	{
		buttonsTime = qmGetTick();
		buttonsState = BS_LOCKED;
	}

	if ((buttonsState == BS_LOCKED) && (qmGetTick() - buttonsTime) > qcfgANTI_BOUNCE_DELAY)
	{
		if ((!HAL_GPIO_ReadPin(VOLUME_DOWN_GPIO_PORT, VOLUME_DOWN_PIN)) &&
			(!HAL_GPIO_ReadPin(VOLUME_UP_GPIO_PORT, VOLUME_UP_PIN)) &&
			(!HAL_GPIO_ReadPin(TRACK_BACK_GPIO_PORT, TRACK_BACK_PIN)) &&
			(!HAL_GPIO_ReadPin(TRACK_NEXT_GPIO_PORT, TRACK_NEXT_PIN)) &&
			(!HAL_GPIO_ReadPin(TRACK_PAUSE_GPIO_PORT, TRACK_PAUSE_PIN)))
			buttonsState = BS_RELEASED;
	}
}
