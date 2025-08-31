/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#ifndef		LOW_LAYER_ADJUNCT_H_
#define		LOW_LAYER_ADJUNCT_H_

#ifdef		__cplusplus
extern		"C" {
#endif
/*  = = = = = = = = = = = = = = = = = = = = = = = */

#include	"string.h"
#include	"stdint.h"
#include	"stdio.h"
#include	"stdlib.h"
#include	"math.h"
#include	"stdarg.h"
#include	"stdbool.h"
#include	"ff.h"

/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *													 Types and Constants
 */
#if	(defined (__x86_64__) ||defined (__aarch64__) || defined (_M_ARM64))
	#define	_long__
	#define	L32__
#else
	#define	_long__					long
#endif

typedef		unsigned char			ui8;
typedef		unsigned short			ui16;
typedef		_long__ unsigned int	ui32;
typedef		long long unsigned int	ui64;

typedef		signed char				i8;
typedef		short signed int		i16;
typedef		_long__ signed int		i32;
typedef		long long signed int	i64;

#ifndef setOFF
	#define	setOFF					(0)
#endif

#ifndef setON
	#define	setON					(-1)
#endif

#define		BIT_0_SET				0x1U
#define		BIT_1_SET				0x2U
#define		BIT_2_SET				0x4U
#define		BIT_3_SET				0x8U
#define		BIT_4_SET				0x10U
#define		BIT_5_SET				0x20U
#define		BIT_6_SET				0x40U
#define		BIT_7_SET				0x80U
#define		BIT_8_SET				0x100U
#define		BIT_9_SET				0x200U
#define		BIT_10_SET				0x400U
#define		BIT_11_SET				0x800U
#define		BIT_12_SET				0x1000U
#define		BIT_13_SET				0x2000U
#define		BIT_14_SET				0x4000U
#define		BIT_15_SET				0x8000U
#define		BIT_16_SET				0x10000U
#define		BIT_17_SET				0x20000U
#define		BIT_18_SET				0x40000U
#define		BIT_19_SET				0x80000U
#define		BIT_20_SET				0x100000U
#define		BIT_21_SET				0x200000U
#define		BIT_22_SET				0x400000U
#define		BIT_23_SET				0x800000U
#define		BIT_24_SET				0x1000000U
#define		BIT_25_SET				0x2000000U
#define		BIT_26_SET				0x4000000U
#define		BIT_27_SET				0x8000000U
#define		BIT_28_SET				0x10000000U
#define		BIT_29_SET				0x20000000U
#define		BIT_30_SET				0x40000000U
#define		BIT_31_SET				0x80000000U

#define		BUFF_SIZE_(buff)		buff, (sizeof(buff))
#define		TEXT_LENGTH_(text)		text, (strlen(text))
#define		DATA_BLOCK_(arr)		{ sizeof((arr)), countof_((arr)) }
#define		VAR_BLOCK_(var)			{ sizeof((var)), 1 }

#define		PACKED__				__attribute__((packed))
#define		WEAK__					__attribute__((weak))
#define		NORETURN__				__attribute__((noreturn))


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *									                     Adjunct Objects
 */
typedef enum
{
	qeFALSE 						= 0U,
	qeTRUE 							= 1U

}	qeBoolean_t;


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *											 Static Functions and Macros
 */
#define		unused_(x)				((void)(x))
#define		donothing_				(void)(1)
#define		nameof_(var)			#var
#define		countof_(arr)			(sizeof(arr) / sizeof(*(arr)))
#define		offsetof_(pEnd, pStart)	(((ui8*)(pEnd)) - ((ui8*)(pStart)))
#define		clearbuff_(p)			(memset(p, 0, sizeof(*(p))))
#define		min_(val, max)			((val) > (max) ? (max) : (val))
#define		max_(val, min)			((val) < (min) ? (min) : (val))
#define		constrain_(val, min, max)\
									((val) < (min) ? (min) : (val) > (max) ? (max) : (val))
#define		rawconstrain_(val, min, max, raw)\
									(((val) <= (min)) || ((val) >= (max)) ? (raw) : (val))
#define		deadbandcross_(val, valOrg, db)\
									((abs((val) - (valOrg)) > (db)) ? 1U : 0U)
#define		toboolean_(val)			((val) && 1U)
#define		lambdafunc_(body)		([](void) { body; } )
#define		calcFreeSpaceFIFO(t, h, max)\
									(((h) >= (t)) ? ((max) - ((h) - (t))) : ((t) - (h)))
#define		memmove_(pd, ps, size)	for (int q = 0; q < size; q++) { pd[q] = ps[q]; }
#define		assign_(v, newV)		if (v != newV) v = newV;


static inline double ConvertRadiansToDegrees (double radian)
{
	return radian * 180.0 / M_PI;
}


static inline double ConvertDegreesToRadians (double degree)
{
	return degree * M_PI / 180.0;
}


static inline ui32 DecodeSyncsafeTo_u32 (ui8* b)
{
	return ((b[0]&0x7F) << 21) | ((b[1]&0x7F) << 14) | ((b[2]&0x7F) << 7) | (b[3]&0x7F);
}


static inline ui16 TransformBigEndianTo16Bit (const ui8 *p)
{
	return (ui16)((p[0] << 8) | p[1]);
}


static inline ui32 TransformBigEndianTo32Bit (const ui8 *p)
{
	return ((ui32)p[0] << 24) | ((ui32)p[1] << 16) | ((ui32)p[2] << 8) | (ui32)p[3];
}


static inline int SafeRead_u8 (FIL* f, ui8 *out)
{
	UINT br;
	if (f_read(f, out, 1, &br) !=FR_OK || br != 1) return -1;
	return 0;
}


static inline int SafeReadBytes (FIL* file, void *buf, UINT len)
{
	UINT br;
	if (f_read(file, buf, len, &br) !=FR_OK || br != len) return -1;
	return 0;
}

static inline size_t TransformCodePointToUTF8 (ui32 cp, char *dst, size_t dstsz)
{
    if (cp <= 0x7F)
    {
    	if (dstsz < 1) return 0;
    	dst[0]=(char)cp; return 1;
    }
    if (cp <= 0x7FF)
    {
    	if (dstsz < 2) return 0;
    	dst[0] = (char)(0xC0 | (cp >> 6)); dst[1] = (char)(0x80 | (cp&0x3F)); return 2;
    }
    if (cp <= 0xFFFF)
    {
    	if (dstsz < 3) return 0;
    	dst[0] = (char)(0xE0 | (cp >> 12)); dst[1] = (char)(0x80 | ((cp >> 6)&0x3F)); dst[2] = (char)(0x80 | (cp&0x3F)); return 3;
    }
    if (dstsz < 4) return 0;
    dst[0] = (char)(0xF0 | (cp >> 18)); dst[1] = (char)(0x80 | ((cp >> 12)&0x3F)); dst[2] = (char)(0x80 | ((cp >> 6)&0x3F)); dst[3] = (char)(0x80 | (cp&0x3F)); return 4;
}

/*  = = = = = = = = = = = = = = = = = = = = = = = */
#ifdef		__cplusplus
			}
#endif

#endif		/* LOW_LAYER_ADJUNCT_H_ */
