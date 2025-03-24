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

#include	"digitalChannels.h"
#include	"SD_Card.h"
#include	"CS43L22.h"

#include	"ff.h"
#include	"ff_gen_drv.h"

#include	"printf.h"

#define		qmCall(method)			(this->*method)()


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *															Object types
 */
typedef enum
{
	uPL_EVENT_NONE					= 0,

	uPL_EVENT_VOLUME_DOWN			= 0x01,
	uPL_EVENT_VOLUME_UP				= 0x02,
	uPL_EVENT_TRACK_BACK			= 0x04,
	uPL_EVENT_TRACK_NEXT			= 0x08,
	uPL_EVENT_TRACK_PAUSE_PLAY		= 0x10

}	uPlayerEvent_t;


typedef enum
{
	uPLFC_NONE						= 0,
	uPLFC_MP3,
	uPLFC_FLAC,
	uPLFC_WAV

}	uPlayerFileCodec_t;


typedef enum
{
	uPL_OK							= 0,
	uPL_ERROR

}	uPlayerStatus_t;


typedef enum
{
	uPL_STOP						= 0,
	uPL_PAUSE,
	uPL_PLAY

}	uPlayerState_t;


typedef enum
{
	uPL_MODE_NORMAL					= 0,
	uPL_MODE_RANDOM					= 0x01,

	uPL_MODE_REPEAT_TRACK			= 0x02,
	uPL_MODE_REPEAT_PLAYLIST		= 0x04

}	uPlayerPlayMode_t;


typedef struct
{
	FATFS		fs;

	FILINFO		fno;
	DIR			dir;

	char		SD_Path [4];

	char		*playlistTrackNames;
	ui16		*playlistTrackNamePointers;

}	qsFatFS_t;


class qc_uPlayer : public qcSD_Card, public qcCS43L22, public qcDigitalChannels
{
	public :

		/* Low layer objects */
		uPlayerEvent_t		event;

		/* FAT FS objects */
		qsFatFS_t			fatFS;


		/* Audio codec objects */
		void*				PCM_Buffer;

		void*				track;
		char*				trackPath;
		ui16				framesRead;

		ui32				trackNumber;
		ui32				totalTrackNumber;

		uPlayerFileCodec_t	currentCodec;

		uPlayerStatus_t		(qc_uPlayer::*OpenTrack)(void);
		uPlayerStatus_t		OpenMP3 (void);
		uPlayerStatus_t		OpenFLAC (void);
		uPlayerStatus_t		OpenWAV (void);

		uPlayerStatus_t		(qc_uPlayer::*ReadFrames)(void);
		uPlayerStatus_t		ReadMP3 (void);
		uPlayerStatus_t		ReadFLAC_16 (void);
		uPlayerStatus_t		ReadFLAC_32 (void);
		uPlayerStatus_t		ReadWAV_16 (void);
		uPlayerStatus_t		ReadWAV_32 (void);

		uPlayerStatus_t		(qc_uPlayer::*CloseTrack)(void);
		uPlayerStatus_t		CloseMP3 (void);
		uPlayerStatus_t		CloseFLAC (void);
		uPlayerStatus_t		CloseWAV (void);

		/* High layer objects */
		uPlayerStatus_t		status;
		uPlayerState_t		state;

		uPlayerPlayMode_t	playMode;

		uPlayerStatus_t		Init (void);

		uPlayerStatus_t		InitFolder (const char *folder);

		uPlayerStatus_t		InitCodec (void);
		uPlayerStatus_t		DeinitCodec (void);

		bool				GetEvent (uPlayerEvent_t event) { return this->event & event; }
		void				SetEvent (uPlayerEvent_t newEvent) { event = (uPlayerEvent_t)(event | newEvent); }
		void				ResetEvent (uPlayerEvent_t goneEvent) { event = (uPlayerEvent_t)(event & ~goneEvent); }
		uPlayerStatus_t		EventHandler (void);

		uPlayerStatus_t		VolumeUp (void);
		uPlayerStatus_t		VolumeDown (void);

		uPlayerStatus_t		Next (void);
		uPlayerStatus_t		Back (void);
		uPlayerStatus_t		Stop (void);
		uPlayerStatus_t		Pause (void);
		uPlayerStatus_t		Play (void);

		uPlayerStatus_t		Task (void);

};

extern qc_uPlayer uPlayer;


/*  = = = = = = = = = = = = = = = = = = = = = = = */

#endif		/* HIGH_LAYER_MICRO_PLAYER_H_ */
