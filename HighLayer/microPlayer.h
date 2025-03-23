/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#ifndef		HIGH_LAYER_MICRO_PLAYER_H_
#define		HIGH_LAYER_MICRO_PLAYER_H_

/*  = = = = = = = = = = = = = = = = = = = = = = = */

#include	"SD_Card.h"
#include	"CS43L22.h"

#include	"ff.h"
#include	"ff_gen_drv.h"


typedef enum
{
	uPL_OK							= 0,
	uPL_ERROR

}	uPlayerStatus_t;


typedef enum
{
	uPL_IDLE						= 0,
	uPL_PAUSE,
	uPL_PLAY

}	uPlayerState_t;


class qc_uPlayer : public qcSD_Card, public qcCS43L22
{
	public :

		i16					PCM_Buffer [qcfgPCM_BUFFER_SIZE];

		uPlayerStatus_t		status;
		uPlayerState_t		state;

		qc_uPlayer			(void)
							{
								bufferOffset = BUFFER_OFFSET_NONE;
							}

		void				Init (void);

		uPlayerStatus_t		VolumeUp (void);
		uPlayerStatus_t		VolumeDown (void);

		uPlayerStatus_t		Back (void);
		uPlayerStatus_t		Pause (void);
		uPlayerStatus_t		Next (void);

		uPlayerStatus_t		Play (void);

		/* Getters - setters */
		uPlayerState_t		GetCurrentState (void);
		void				SetCurrentState (uPlayerState_t newState);

};


#endif		/* HIGH_LAYER_MICRO_PLAYER_H_ */
