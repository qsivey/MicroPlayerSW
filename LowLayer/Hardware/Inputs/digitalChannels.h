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


typedef enum
{
	BS_RELEASED						= 0,
	BS_PRESSED,
	BS_LOCKED

}	qeButtonsState_t;


class qcDigitalChannels
{
	public :

		qeButtonsState_t	buttonsState;
		ui32				buttonsTime;

		void				ButtonsInit (void);
		void				ButtonsHandle (void);

};


/*  = = = = = = = = = = = = = = = = = = = = = = = */

#endif		/* INPUTS_DIGITAL_CHANNELS_H_ */
