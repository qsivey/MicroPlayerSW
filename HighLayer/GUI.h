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

/* Base GUI parameters */
#define 	GUI_DISPLAY_WIDTH		240
#define 	GUI_DISPLAY_HEIGHT		240

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

class qc_GUI : public qcST7789
{
	public :

		/* Low layer objects */


		/* Text */
		char				printBuf [qcfgPRINT_BUF_SIZE];



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
