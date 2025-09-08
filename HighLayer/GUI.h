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

#define 	MAX_COMPONENTS 			5
#define     MAX_LANGUAGES  			3 //2 languages + button "back"

#define		MAIN_SCREEN_OBJ_NUM		10;

typedef enum
{
	GUIOT_RECTANGLE					= 0,
	GUIOT_IMAGE,
	GUIOT_STRING,
	GUIOT_VOLUME_BAR,
	GUIOT_DURATION_BAR,
	GUIOT_WIDGET

}	qeGUI_ObjectType_t;


typedef struct
{
	tFont	*font;
	char	*str;
	ui8		length;

	i16		scrollPhase;
	ui16	xEnd,
			yEnd;

	ui16	color,
			bgColor;

	bool	background;

}	qsGUI_StringParams_t;


typedef struct
{
	/* Low layer variables */
	ui16			*buff,

					x,
					y,
					width,
					height;

	bool			visible;
	ui8				opacity;						// непрозрачность

	bool			needUpdate;

	/* High layer variables */
	qeGUI_ObjectType_t
					type;

	void			*params;

	void			*customFunc;

}	qsGUI_Object_t;


typedef struct
{
	qsGUI_Object_t	WgSampleRate,
					WgBitRate,
					WgTrackFmt,
					WgPlayMode,

					StrBatLevel,
					StrTrackTitle,
					StrTrackArtist,
					StrPlayTime,
					StrTrackDrt,

					ImgTrackPicture,

					ObjVolumeBar,
					ObjDurationBar;

	ui8				objectNumber;

}	qsGUI_MainScreen_t;


typedef struct
{
	ui8				index;
	ui8				subindex;
	ui8				where;

	char			highlayer [MAX_COMPONENTS][MAX_NAME_LEN];
	char			lang [MAX_LANGUAGES][MAX_NAME_LEN];

}	qsmenu_t;


typedef	void		(*qtGUI_FillRect_t)(ui16, ui16, ui16, ui16, ui16);
typedef	void		(*qtGUI_FillBuff_t)(ui16, ui16, ui16, ui16, ui16*);


class qc_GUI : public qcST7789, public qcRealTimeClock, public qcParse
{
	public :

		/* Low layer objects */
		ui16				displayBuffer [qcfgDISPLAY_WIDTH * qcfgDISPLAY_HEIGHT];

		qsGUI_MainScreen_t	MainScreen;

		void				FillRect (ui16 x , ui16 y, ui16 width, ui16 height, ui16 color)
							{
								ST7789_DrawFilledRectangle(x, y, width, height, color);
							}

		void				FillBuff (ui16 x , ui16 y, ui16 width, ui16 height, ui16 *buff)
							{
								ST7789_DrawPartofImage(x, y, width, height, buff);
							}

		void				GUI_Init (void);
		void				GUI_InitMainScreen (void);

		void				GUI_InitObject (qsGUI_Object_t *obj,
								 	 		ui16 x, ui16 y, ui16 width, ui16 height,
											ui8 opacity, qeGUI_ObjectType_t type,
											void *customFunc);
		void				GUI_ShowObject (qsGUI_Object_t *obj);
		void				GUI_HideObject (qsGUI_Object_t *obj);
		void				GUI_MoveObject (qsGUI_Object_t *obj, ui16 newX, ui16 newY);

		void				GUI_HandleObjects (void);
		void				GUI_RedrawObject (qsGUI_Object_t *obj);

		void				GUI_InitString (qsGUI_Object_t *obj, tFont *font, ui16 xEnd, ui16 yEnd);
		void				GUI_RenderString (qsGUI_Object_t *obj, char *str, ui16 color, ui16 bgColor, bool background);

		void				InitImage (qsGUI_Object_t *obj);
		void				SetImage (qsGUI_Object_t *obj, ui16 *buffer);

		void				InitFirstDisplay (void);

		/* First panel methods */
		/* First level */
		void				HideBottomPanel (void);

		ui32				volumeBarTick;
		bool				volumeBarState;
		bool				flagVolBar = false;
		void				ShowVolumeBar (ui8 volume);
		void				UpdateVolumeBar (ui8 volume);
		void				HideVolumeBar (void);

		/* Second level */
		void 				PrintRender (void);

		void				DrawAttributes (void);
		ui16				batteryLevel;

		void				PrintTrackInfo (void);
		void				PrintTrackPicture (const TCHAR* path, char original_path);

		ui32				startTime;
		void				PrintTrackTime (void);
		void				PrintTrackCurrentTime (void);

		/* Menu */
		qsmenu_t			menu;
		char 				about [128];

		void				InitMenu (ui8 selected);
		void				DrawMenu (ui8 selected);
		void				DrawFolderMenu (uint8_t selected);
		void				Language (ui8 selected);
		void				SelectLang (ui8 selected);
		void				Transition (ui8 selected);
		void				About (void);

		/* Helpers */

		ui16				Mix565 (ui16 color, ui16 bgColor, ui8 alpha);
		inline ui16			RGB565 (ui8 r, ui8 g, ui8 b);

		ui32				UTF8MergeCode (char **ptr);
		const tChar*		FindChar (const tFont *font, i32 code);
		const tImage*		FindCharImage (const tFont *font, long int code);

		void				DrawGlyphToBuf (qsGUI_Object_t *obj,  const tImage *img);
};


/*  = = = = = = = = = = = = = = = = = = = = = = = */

#endif		/* HIGH_LAYER_GUI_H_ */
