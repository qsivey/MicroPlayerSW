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
#define		qcfgANTI_BOUNCE_DELAY	200				// Units: ms

#define		qcfgHAL_TIMEOUT_DELAY	100				// Units: ms

#define		qcfgCS43L22_I2C_ADDR	0x94

#define		DR_FLAC_IMPLEMENTATION
//#define		DR_MP3_IMPLEMENTATION
#define		DR_WAV_IMPLEMENTATION


/* -------------------------------------------------
 * High Layer Configs
 */
#define		qcfgPCM_BUFFER_SIZE		8192			// Units: bytes * type. 1024 --> 16382 (i16 : x2, i32: x4). 65535 Max

#define		qcfgBACKLIGHT_TIMER		100000			// Units: ms

#define		qcfgPRINT_BUF_SIZE		64

#define		qcfgMAX_VOLUME			100				// 1 --> 100
#define		qcfgMIN_VOLUME			0				// 0 --> 99
#define		qcfgINITIAL_VOLUME		70				// 0 --> 100
#define		qcfgVOLUME_PITCH		4				// 1 --> 100


/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

#endif		/* HIGH_LAYER_PROJECT_CONFIG_H_ */
