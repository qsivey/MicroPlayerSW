/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#include    "GUI.h"


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   Class Methods
 */
void qc_GUI::SetPictureMusic (void)
{
	;
}


void qc_GUI::ShowBottomPanel (void)
{
	ST7789_DrawFilledRectangle(	0, GUI_DISPLAY_HEIGHT - GUI_BOT_PANNEL_HEIGHT,
								GUI_DISPLAY_WIDTH, GUI_BOT_PANNEL_HEIGHT, GRAY);
	ST7789_DrawRectangle(10, GUI_DISPLAY_HEIGHT - GUI_BOT_PANNEL_HEIGHT+6,10+240-80,
							GUI_DISPLAY_HEIGHT - GUI_BOT_PANNEL_HEIGHT+14+6, 1, YELLOW);

	ST7789_DrawFilledCircle(100, GUI_DISPLAY_HEIGHT - GUI_BOT_PANNEL_HEIGHT+6+7, 5, YELLOW);

	ST7789_WriteString(10+240-80+10,240-60+2, (char*)"1:43",&Font20,GRAY,WHITE);
	ST7789_WriteString(10,180+26, (char*)"name", &Font20,GRAY,WHITE);
}


void qc_GUI::HideBottomPanel (void)
{
	ST7789_DrawFilledRectangle(	0, GUI_DISPLAY_HEIGHT - GUI_BOT_PANNEL_HEIGHT,
								GUI_DISPLAY_WIDTH, GUI_BOT_PANNEL_HEIGHT, BLACK);
}


void qc_GUI::ShowVolumeBar (ui8 volume)
{
	if (!flagVolBar)
	{
		ST7789_DrawRectangle(4, 20,
								4 + GUI_VOLUME_BAR_WIDTH + 3 * 2 + 2 * GUI_VOLUME_BAR_THICK,
								20 + GUI_VOLUME_BAR_HEIGHT + 3 * 2 + 2 * GUI_VOLUME_BAR_THICK,
								GUI_VOLUME_BAR_THICK, WHITE);

		flagVolBar = true;
	}

	UpdateVolumeBar(volume);
}


void qc_GUI::UpdateVolumeBar (ui8 volume)
{
	ST7789_DrawFilledRectangle(	GUI_VOLUME_BAR_X_COORD,	GUI_VOLUME_BAR_Y_COORD,
								GUI_VOLUME_BAR_WIDTH, 80 - (GUI_VOLUME_BAR_INDENT * volume), BLACK);

	volume ?
	ST7789_DrawFilledRectangle(	GUI_VOLUME_BAR_X_COORD,	GUI_VOLUME_BAR_Y_COORD + (80 - (GUI_VOLUME_BAR_INDENT * volume)),
								GUI_VOLUME_BAR_WIDTH, GUI_VOLUME_BAR_INDENT * volume, WHITE)
								: donothing_;

	volumeBarTick = qmGetTick();
}


void qc_GUI::HideVolumeBar (void)
{
	ST7789_DrawFilledRectangle(	4,
								20,
								4 + GUI_VOLUME_BAR_WIDTH+3*2+2*GUI_VOLUME_BAR_THICK,
								20 + GUI_VOLUME_BAR_HEIGHT+3*2+2*GUI_VOLUME_BAR_THICK, BLACK);

	flagVolBar = false;
}


void qc_GUI::DrawAttributes (void)
{

}
void qc_GUI::DrawDurationBar (void)
{

}
void qc_GUI::PrintTrackInfo (void)
{

}
