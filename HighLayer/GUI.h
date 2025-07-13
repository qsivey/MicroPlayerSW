/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#ifndef		HIGH_LAYER_GUI_H_
#define		HIGH_LAYER_GUI_H_

/*  = = = = = = = = = = = = = = = = = = = = = = = */


#include    "ST7789.h"

#include	"ff.h"
#include	"ff_gen_drv.h"

#include	"printf.h"
#include	"tjpgd.h"

/* Base GUI parameters */
#define 	GUI_DISPLAY_WIDTH		qcfgDISPLAY_WIDTH
#define 	GUI_DISPLAY_HEIGHT		qcfgDISPLAY_HEIGHT

/* Bottom Panel */
#define		GUI_BOT_PANNEL_HEIGHT	60

/* Widget parameters */
#define		GUI_WIDGET_SIZE			32
#define		GUI_WIDGET_HOR_PAD		4
#define		GUI_WIDGET_VERT_PAD		4

/* Volume Bar */
#define 	GUI_VOLUME_BAR_WIDTH	8
#define 	GUI_VOLUME_BAR_HEIGHT	80

#define		GUI_VOLUME_BAR_INDENT	(GUI_VOLUME_BAR_HEIGHT / (qcfgMAX_VOLUME - qcfgMIN_VOLUME))

#define		GUI_VOLUME_BAR_INT_PAD	4
#define 	GUI_VOLUME_BAR_THICK	2

#define		GUI_VOLUME_BAR_SIDE_PAD	4
#define		GUI_VOLUME_BAR_TOP_PAD	4

#define		GUI_VOLUME_BAR_X_COORD	4 + 3 + GUI_VOLUME_BAR_THICK  //(GUI_DISPLAY_WIDTH - GUI_VOLUME_BAR_SIDE_PAD - GUI_VOLUME_BAR_WIDTH)
#define		GUI_VOLUME_BAR_Y_COORD	20 + 3 + GUI_VOLUME_BAR_THICK //(GUI_WIDGET_SIZE + (2 * GUI_WIDGET_VERT_PAD) + GUI_VOLUME_BAR_TOP_PAD + GUI_VOLUME_BAR_HEIGHT)


#pragma pack(push, 1)

typedef struct
{
	ui16			bfType;							// 'BM'
	uint32_t		bfSize;
	ui16			bfReserved1;
	ui16			bfReserved2;
	uint32_t		bfOffBits;

}	qsBMP_Header_t;


typedef struct
{
    uint32_t		biSize;
    int32_t			biWidth;
    int32_t			biHeight;
    ui16			biPlanes;
    ui16			biBitCount;
    uint32_t		biCompression;
    uint32_t		biSizeImage;
    int32_t			biXPelsPerMeter;
    int32_t			biYPelsPerMeter;
    uint32_t		biClrUsed;
    uint32_t		biClrImportant;

}	qsBMP_InfoHeader_t;

#pragma pack(pop)


typedef struct
{
	FIL		file;
	JDEC	jd;
	ui8		tjdBuff [qcfgTJD_BUFF_SIZE];

}	qsJPEG_t;


class qc_GUI : public qcST7789
{
	public :

		/* Low layer objects */
		ui16				displayBuffer [qcfgDISPLAY_WIDTH * qcfgDISPLAY_HEIGHT];

		/* Text */
		char				printBuf [qcfgPRINT_BUF_SIZE];

		/* JPEG */
		qsJPEG_t			*JPEG_Temp;

		static size_t		InputJPEG (JDEC* jd, uint8_t* buff, size_t len);
		static int			OutputJPEG (JDEC *jd, void *bitmap, JRECT *rect);
		int					ScaleJPEG (void *bitmap, JRECT *rect);
		bool				RenderJPEG (void);

		/* Cache */
		void				CacheDisplay (const char *filename);

		/* First panel methods */
		/* First level */
		void				SetPictureMusic (void);

		void				ShowBottomPanel (void);
		void				HideBottomPanel (void);

		ui32				volumeBarTick;
		bool				volumeBarState;
		bool				flagVolBar = false;
		void				ShowVolumeBar (ui8 volume);
		void				UpdateVolumeBar (ui8 volume);
		void				HideVolumeBar (void);

		/* Second level */
		void				DrawAttributes (void);
		void				DrawDurationBar (void);
		void				PrintTrackInfo (void);

};


/*  = = = = = = = = = = = = = = = = = = = = = = = */

#endif		/* HIGH_LAYER_GUI_H_ */
