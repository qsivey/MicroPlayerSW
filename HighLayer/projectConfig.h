/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#ifndef		HIGH_LAYER_PROJECT_CONFIG_H_
#define		HIGH_LAYER_PROJECT_CONFIG_H_

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

#include	"adjunct.h"


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														 Project Configs
 */
/* -------------------------------------------------
 * Low Layer Configs
 */
#define		qcfgDISPLAY_WIDTH		240				// Units: pixel
#define		qcfgDISPLAY_HEIGHT		240				// Units: pixel

#define		qcfgPICTURE_RESOLUTION	240				// Units: pixel

#define		qcfgTJD_BUFF_SIZE		(1024 * 64)		// Units: bytes

#define		qcfgANTI_BOUNCE_DELAY	200				// Units: ms

#define		qcfgHAL_TIMEOUT_DELAY	100				// Units: ms

#define		DR_FLAC_IMPLEMENTATION
#define		DR_MP3_IMPLEMENTATION
#define		DR_WAV_IMPLEMENTATION

/* -------------------------------------------------
 * High Layer Configs
 */
#define		qcfgPCM_BUFFER_SIZE		(1024 * 16)		// Units: bytes * size. 1024 --> 32768

#define		qcfgVOLUME_BAR_TIMER	2000			// Units: ms

#define		qcfgBACKLIGHT_TIMER		100000			// Units: ms

#define		qcfgPRINT_BUF_SIZE		64

#define		qcfgMAX_VOLUME			16				// 1 --> 16
#define		qcfgMIN_VOLUME			0				// 0 --> 15
#define		qcfgINITIAL_VOLUME		12				// 0 --> 16

#define		qcfgCACHE_IMG_PATH		"0:/cache/img"


/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

#endif		/* HIGH_LAYER_PROJECT_CONFIG_H_ */
