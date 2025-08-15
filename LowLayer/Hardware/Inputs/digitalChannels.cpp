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


static qsButtons_t ButtonStateReferenceArray [] =

	{ {	BUTTONS_GPIO_PORT,		VOLUME_DOWN_PIN,		BS_RELEASED,		0,		NULL,	NULL,	NULL
	},{	BUTTONS_GPIO_PORT,		VOLUME_UP_PIN,			BS_RELEASED,		0,		NULL,	NULL,	NULL
	},{	BUTTONS_GPIO_PORT,		TRACK_PLAY_PIN,			BS_RELEASED,		0,		NULL,	NULL,	NULL
	},{	BUTTONS_GPIO_PORT,		TRACK_BACK_PIN,			BS_RELEASED,		0,		NULL,	NULL,	NULL
	},{	BUTTONS_GPIO_PORT,		TRACK_NEXT_PIN,			BS_RELEASED,		0,		NULL,	NULL,	NULL

	}};


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

	Buttons = ButtonStateReferenceArray;
}


void qcDigitalChannels::ButtonsHandle (void)
{
	for (ui8 i = 0; i < BUT_NUMBER; i++)
	{
		switch (Buttons[i].status)
		{
			case BS_ANTI_BOUNCE :
			{
				if ((qmGetTick() - Buttons[i].pressTime) > qcfgANTI_BOUNCE_DELAY)
				/* It's time to check button state one more time */
				{
					if (HAL_GPIO_ReadPin(Buttons[i].gpio, Buttons[i].pin))
						Buttons[i].status = BS_LOCKED;

					else
						Buttons[i].status = BS_RELEASED;
				}

				break;
			}

			case BS_LOCKED :
			{
				if (HAL_GPIO_ReadPin(Buttons[i].gpio, Buttons[i].pin))
				{
					if ((qmGetTick() - Buttons[i].pressTime) > qcfgBUTTON_HOLD_DELAY)
						Buttons[i].status = BS_HOLD;
				}

				else
					Buttons[i].status = BS_PRESSED;

				break;
			}

			case BS_HOLD :
			{
				if (HAL_GPIO_ReadPin(Buttons[i].gpio, Buttons[i].pin))
					Buttons[i].OnHold(Buttons[i].pThis);

				else
				{
					Buttons[i].OnLastHold(Buttons[i].pThis);
					Buttons[i].status = BS_RELEASED;
				}


				break;
			}

			case BS_PRESSED :
			{
				Buttons[i].OnPress(Buttons[i].pThis);

				Buttons[i].status = BS_RELEASED;

				break;
			}

			default :
				break;
		}
	}
}
