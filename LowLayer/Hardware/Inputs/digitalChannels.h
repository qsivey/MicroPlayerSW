/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#ifndef		INPUTS_DIGITAL_CHANNELS_H_
#define		INPUTS_DIGITAL_CHANNELS_H_

/*  = = = = = = = = = = = = = = = = = = = = = = = */

#include	"periph.h"


typedef		void					(*qfButtonHandler_t)(void *param);


typedef enum
{
	BUT_VOL_DOWN					= 0,
	BUT_VOL_UP,
	BUT_TRACK_PLAY,
	BUT_TRACK_BACK,
	BUT_TRACK_NEXT,

	BUT_NUMBER

}	qeButtons_t;


typedef enum
{
	BS_RELEASED						= 0,
	BS_ANTI_BOUNCE,
	BS_LOCKED,
	BS_PRESSED,
	BS_HOLD

}	qeButtonStatus_t;


typedef struct
{
	GPIO_TypeDef			*gpio;
	ui32					pin;

	qeButtonStatus_t		status;
	ui32					pressTime;

	qfButtonHandler_t		OnPress,
							OnHold,
							OnLastHold;

	void					*pThis;

}	qsButtons_t;


class qcDigitalChannels
{
	public :

		qsButtons_t			*Buttons;

		void				ButtonsInit (void);
		void				ButtonsHandle (void);

};


/*  = = = = = = = = = = = = = = = = = = = = = = = */

#endif		/* INPUTS_DIGITAL_CHANNELS_H_ */
