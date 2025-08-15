/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#ifndef		MISC_REALTIME_CLOCK_H_
#define		MISC_REALTIME_CLOCK_H_

/*  = = = = = = = = = = = = = = = = = = = = = = = */

#include	"periph.h"

typedef struct {
    RTC_TimeTypeDef	startTime;
    uint32_t 		totalDurationSec;
} player_timer_t;

class RealTimeClock
{
	private:

		RTC_HandleTypeDef 	hrtc;

	public :

		player_timer_t 		playerTime;

		void				RTC_Init (void);
		void				StartTimer(char* str);
		ui32				GetElapsedTime(void);

};


/*  = = = = = = = = = = = = = = = = = = = = = = = */

#endif		/* MISC_REALTIME_CLOCK_H_ */
