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
 *														   		  Extern
 */

extern uint16_t image_data_BR12[];
extern uint16_t image_data_BR16[];
extern uint16_t image_data_BR24[];
extern uint16_t image_data_BR32[];
extern uint16_t image_data_SF96[];
extern uint16_t image_data_SF192[];
extern uint16_t image_data_SF32[];
extern uint16_t image_data_SF384[];
extern uint16_t image_data_SF44[];
extern uint16_t image_data_SF48[];
extern uint16_t defaultPicture[];


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   Class Methods
 */
void qc_GUI::SetPictureMusic (void)
{
	ST7789_DrawPartofImage(0, 0, 240, 240, defaultPicture);
}


void qc_GUI::HideBottomPanel (void)
{
	ST7789_DrawFilledRectangle(0, GUI_DISPLAY_HEIGHT - GUI_BOT_PANNEL_HEIGHT,
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
	if (Metadata.sampleRate == 44100)  ST7789_DrawPartofImage(0, 0, 30, 19, image_data_SF44);
	if (Metadata.sampleRate == 48000)  ST7789_DrawPartofImage(0, 0, 30, 19, image_data_SF48);
	if (Metadata.sampleRate == 192000) ST7789_DrawPartofImage(0, 0, 30, 19, image_data_SF192);
	if (Metadata.sampleRate == 32000)  ST7789_DrawPartofImage(0, 0, 30, 19, image_data_SF32);

	char batlev[16];
	batteryLevel *= 0.02454;
	sprintf(batlev, "%02d%", (int)batteryLevel);
	ST7789_WriteString(190, 2, batlev, &Font16, WHITE, BLACK);

	ST7789_WriteString(193, 220, "pause", &Font16, WHITE, BLACK);
}


void qc_GUI::PrintTrackInfo (void)
{
	ST7789_FillColor(BLACK);

	ST7789_WriteString(10, 195, Metadata.title, &Font20, WHITE, BLACK);
	ST7789_WriteString(10, 220, Metadata.artist, &Font16, WHITE, BLACK);
}

void qc_GUI::PrintRender (void)
{
	uint16_t *buf = NULL;
	int w, h;
	int rc = RenderStringToRGB565BufferLen((const uint8_t*)"00:01", strlen("00:01"), &Font20,
	                                       WHITE, BLACK,
	                                       &buf, &w, &h);

	if (rc == 0)  DrawBufferToDisplay(10, 20, w, h, buf);
	qmFree(buf);
}

void qc_GUI::PrintTrackTime (void)
{
	float durationSec = 0;
	ui32 seconds = 0, minutes = 0;
	char timeStr[16];

	if (Metadata.existsFlag)
	{

		if (Metadata.format == uAF_FLAC) durationSec = (float)Metadata.totalSamples / Metadata.sampleRate;
		if (Metadata.format == uAF_MP3)  durationSec = (float)Metadata.durationSec;
		if (Metadata.format == uAF_WAV)  durationSec = (float)Metadata.durationSec;

		seconds = (ui32)durationSec;
		minutes = seconds / 60;
		seconds %= 60;

		sprintf(timeStr, "%02d:%02d", minutes, seconds);
		ST7789_WriteString(193, 220, timeStr, &Font16, WHITE, BLACK);
	}
}


//void qc_GUI::PrintTrackCurrentTime(void)
//{
//	ui32 seconds = 0, minutes = 0;
//	char timeStr[16];
//	ui32 curTime = (qmGetTick() - startTime) / 1000;
//
//	seconds = curTime;
//	minutes = seconds / 60;
//	seconds %= 60;
//
//	sprintf(timeStr, "%02d:%02d", minutes, seconds);
//	ST7789_WriteString(185, 195, timeStr, &Font20, WHITE, BLACK);
//}


void qc_GUI::InitMenu(ui8 selected)
{
	if (selected == 0){
		strcpy(menu.highlayer[0], "Font");
		strcpy(menu.highlayer[1], "Folders");
		strcpy(menu.highlayer[2], "Language");
		strcpy(menu.highlayer[3], "Sleep mode");
		strcpy(menu.highlayer[4], "About/Help");
		strcpy(menu.lang[2],"Back");
		strcpy(about,"MicroPlayer project GitHub: qsivey, Nik125Y Telegram: @qsivey, @Nik125Y Email: qsivey@gmail.com, topnikm@gmail.com");
	}
	if (selected == 1){
		strcpy(menu.highlayer[0], "Шрифт");
		strcpy(menu.highlayer[1], "Папки");
		strcpy(menu.highlayer[2], "Язык");
		strcpy(menu.highlayer[3], "Режим сна");
		strcpy(menu.highlayer[4], "О нас/Помощь");
		strcpy(menu.lang[2],"Назад");
		strcpy(about,"Проект Микроплеер GitHub: qsivey, Nik125Y Телеграм: @qsivey, @Nik125Y Почта: qsivey@gmail.com, topnikm@gmail.com");
	}
	strcpy(menu.lang[0],"English");
	strcpy(menu.lang[1],"Русский");
}


void qc_GUI::DrawMenu(ui8 selected){
	ST7789_FillColor(BLACK);

	for (uint8_t i = 0; i < MAX_COMPONENTS; i++) {
		uint16_t color = (i == selected) ? YELLOW : WHITE;
		ST7789_WriteString(5, i * 23, menu.highlayer[i], &Font20, color, BLACK);
	}
}


void qc_GUI::DrawFolderMenu(ui8 selected)
{
	ST7789_FillColor(BLACK);

	for (uint8_t i = 0; i < folderCount; i++) {
		uint16_t color = (i == selected) ? YELLOW : WHITE;
		ST7789_WriteString(5, i * 23, folders[i], &Font20, color, BLACK);
	}
}


void qc_GUI::Language(ui8 selected){
	ST7789_FillColor(BLACK);
	for (uint8_t i = 0; i < 3; i++) {
		uint16_t color = (i == (selected)) ? YELLOW : WHITE;
		ST7789_WriteString(5, i * 23, menu.lang[i], &Font20, color, BLACK);
	}
}


void qc_GUI::SelectLang(ui8 selected)
{
	if (selected == 2){
		menu.where = 0;
		menu.subindex = 0;
		DrawMenu(menu.index);
	}
	else {
		InitMenu(selected);
		Language(selected);
	}
}


void qc_GUI::Transition(ui8 selected)
{
	switch (selected)
	{
		case 0:

			break;
		case 1:
			menu.where = 1;
			menu.subindex = 0;
			DrawFolderMenu(menu.subindex);
			break;
		case 2:
			menu.where = 2;
			menu.subindex = 0;
			Language(menu.subindex);
			break;
		case 3:
			break;
		case 4:
			menu.where = 4;
			About();
			break;
		default :
			break;
	}
}


void qc_GUI::About(void)
{
	ST7789_FillColor(BLACK);
	ST7789_WriteString(0, 4, about, &Font20, WHITE, BLACK);
	ST7789_WriteString(5, 200, menu.lang[2], &Font20, YELLOW, BLACK);
}

