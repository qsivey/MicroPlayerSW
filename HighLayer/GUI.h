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
#include    "parse.h"
#include	"tjpgd.h"
#include	"rtc.h"

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

#define 	MAX_COMPONENTS 5
#define     MAX_LANGUAGES  3 //2 languages + button "back"


#pragma pack(push, 1)

typedef struct
{
	ui16			bfType;							// 'BM'
	ui32			bfSize;
	ui16			bfReserved1;
	ui16			bfReserved2;
	ui32			bfOffBits;

}	qsBMP_Header_t;


typedef struct
{
	ui32			biSize;
    i32				biWidth;
    i32				biHeight;
    ui16			biPlanes;
    ui16			biBitCount;
    ui32			biCompression;
    ui32			biSizeImage;
    i32				biXPelsPerMeter;
    i32				biYPelsPerMeter;
    ui32			biClrUsed;
    ui32			biClrImportant;

}	qsBMP_InfoHeader_t;

#pragma pack(pop)

typedef struct {
	bool 			flag;
    // STREAMINFO
    ui32 			sampleRate;
    ui64 			totalSamples;
    ui8				channels;
    ui8				bitsPerSample;

    // VORBIS_COMMENT
    char 			title[64];
    char 			artist[64];

    // PICTURE
    ui32 			pictureOffset;
    ui32 			pictureSize;
    ui32 			pictureWidth;
    ui32 			pictureHeight;
}	qcFlac_metadata_t;


typedef struct {
	bool 			flag;

	// STREAMINFO
    ui32 			sampleRate;
    ui32 			bitrate;
    float 			duration_sec;

	// VORBIS_COMMENT
    char 			title[64];
    char 			artist[64];
} 	qcMP3_metadata_t;


typedef struct
{
	FIL		file;
	JDEC	jd;
	ui8		tjdBuff [qcfgTJD_BUFF_SIZE];

}	qsJPEG_t;


typedef struct{
	ui8			index;
	ui8			subindex;
	ui8			where;

	char		highlayer[MAX_COMPONENTS][MAX_NAME_LEN];
	char		lang[MAX_LANGUAGES][MAX_NAME_LEN];

}	qsmenu_t;


class qc_GUI : public qcST7789, public RealTimeClock, public qcParse
{
	public :

		/* Low layer objects */
		ui16				displayBuffer [qcfgDISPLAY_WIDTH * qcfgDISPLAY_HEIGHT];

		/* Text */
		char				printBuf [qcfgPRINT_BUF_SIZE];

		/* JPEG */
		qsJPEG_t			*JPEG_Temp;
		bool				flagOutOfImage;

		static size_t		InputJPEG (JDEC* jd, uint8_t* buff, size_t len);
		static int			OutputJPEG (JDEC *jd, void *bitmap, JRECT *rect);
		int					ScaleJPEG (void *bitmap, JRECT *rect);
		bool				RenderJPEG (void);

		/* Meta */

		void				FlacMetaInfo(const TCHAR* path);
		void				MP3MetaInfo(const TCHAR* path);
		void				WAVMetaInfo(const TCHAR* path);

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
		RenderedText 		titleBuf;
		RenderedText 		artistBuf;
		ScrollTextState 	scrollTitle;
		ScrollTextState 	scrollArtist;
		void				DrawAttributes (void);
		ui16				batteryLevel;

		void				PrintTrackInfo (void);
		void				PrintTrackPicture (const TCHAR* path, char original_path);

		ui32				startTime;
		void				PrintTrackTime (void);
		void				PrintTrackCurrentTime (void);

		/* Menu */
		qsmenu_t			menu;
		char 				about[128];

		void				InitMenu (ui8 selected);
		void				DrawMenu (ui8 selected);
		void				DrawFolderMenu (uint8_t selected);
		void				Language (ui8 selected);
		void				SelectLang (ui8 selected);
		void				Transition (ui8 selected);
		void				About (void);

};


/*  = = = = = = = = = = = = = = = = = = = = = = = */

#endif		/* HIGH_LAYER_GUI_H_ */
