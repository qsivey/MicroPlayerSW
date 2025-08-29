/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#ifndef		MISC_RTC_H_
#define		MISC_RTC_H_

/*  = = = = = = = = = = = = = = = = = = = = = = = */

#include	"periph.h"


typedef struct
{
    RTC_TimeTypeDef	startTime;
    ui32			totalDurationSec;

} player_timer_t;


class qcRealTimeClock : virtual public qcPeripheral
{
	public :

		player_timer_t 		playerTime;

		void				RTC_Init (void);
		void				StartTimer (void);
		ui32				GetElapsedTime (void);

};


/*  = = = = = = = = = = = = = = = = = = = = = = = */

#endif		/* MISC_RTC_H_ */
