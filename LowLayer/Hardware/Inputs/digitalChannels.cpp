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

	BUTTONS_GPIO_CLOCK_ENABLE();

	GPIO_InitStruct.Pin = VOLUME_DOWN_PIN | VOLUME_UP_PIN | TRACK_PLAY_PIN | TRACK_BACK_PIN | TRACK_NEXT_PIN;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;

	HAL_GPIO_Init(BUTTONS_GPIO_PORT, &GPIO_InitStruct);

	/* Enable interrupts */
	HAL_NVIC_SetPriority(VOLUME_DOWN_IRQN, 1, 0);
	HAL_NVIC_EnableIRQ(VOLUME_DOWN_IRQN);

	HAL_NVIC_SetPriority(VOLUME_UP_IRQN, 1, 0);
	HAL_NVIC_EnableIRQ(VOLUME_UP_IRQN);

	HAL_NVIC_SetPriority(TRACK_PLAY_IRQN, 1, 0);
	HAL_NVIC_EnableIRQ(TRACK_PLAY_IRQN);

	HAL_NVIC_SetPriority(TRACK_BACK_IRQN, 1, 0);
	HAL_NVIC_EnableIRQ(TRACK_BACK_IRQN);

	HAL_NVIC_SetPriority(TRACK_NEXT_IRQN, 1, 0);
	HAL_NVIC_EnableIRQ(TRACK_NEXT_IRQN);

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
		if ((!HAL_GPIO_ReadPin(BUTTONS_GPIO_PORT, VOLUME_DOWN_PIN)) &&
			(!HAL_GPIO_ReadPin(BUTTONS_GPIO_PORT, VOLUME_UP_PIN)) &&
			(!HAL_GPIO_ReadPin(BUTTONS_GPIO_PORT, TRACK_BACK_PIN)) &&
			(!HAL_GPIO_ReadPin(BUTTONS_GPIO_PORT, TRACK_NEXT_PIN)) &&
			(!HAL_GPIO_ReadPin(BUTTONS_GPIO_PORT, TRACK_PLAY_PIN)))
			buttonsState = BS_RELEASED;
	}
}
