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
extern uint16_t image_data_BR12 [];
extern uint16_t image_data_BR16 [];
extern uint16_t image_data_BR24 [];
extern uint16_t image_data_BR32 [];
extern uint16_t image_data_SF96 [];
extern uint16_t image_data_SF192 [];
extern uint16_t image_data_SF32 [];
extern uint16_t image_data_SF384 [];
extern uint16_t image_data_SF44 [];
extern uint16_t image_data_SF48 [];
extern uint16_t defaultPicture [];


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   Class Methods
 */

/* -------------------------------------------------
 * GUI General
 */
void qc_GUI::GUI_Init (void)
{
	GUI_InitMainScreen();
}


void qc_GUI::GUI_InitMainScreen (void)
{
	MainScreen.objectNumber = MAIN_SCREEN_OBJ_NUM;

	/* TITLE, ARTIST, TIME */
	GUI_InitObject(&MainScreen.StrTrackTitle, 10, 195, 0, 0, 100, GUIOT_STRING, NULL);
	GUI_InitString(&MainScreen.StrTrackTitle, &Font20, 200, 220);

	GUI_InitObject(&MainScreen.StrTrackArtist, 10, 220, 0, 0, 100, GUIOT_STRING, NULL);
	GUI_InitString(&MainScreen.StrTrackArtist, &Font16, 200, 240);

	GUI_InitObject(&MainScreen.StrPlayTime, 200, 195, 0, 0, 100, GUIOT_STRING, NULL);
	GUI_InitString(&MainScreen.StrPlayTime, &Font20, 240, 240);

	GUI_InitObject(&MainScreen.StrTrackDrt, 200, 220, 0, 0, 100, GUIOT_STRING, NULL);
	GUI_InitString(&MainScreen.StrTrackDrt, &Font16, 240, 240);

	/* WIDGETS */
	GUI_InitObject(&MainScreen.WgSampleRate, 0, 0, 30, 19, 100, GUIOT_IMAGE, NULL);
	InitImage(&MainScreen.WgSampleRate);

	GUI_InitObject(&MainScreen.WgBitRate, 30, 0, 30, 19, 100, GUIOT_IMAGE, NULL);
	InitImage(&MainScreen.WgBitRate);

	GUI_InitObject(&MainScreen.WgTrackFmt, 60, 0, 30, 19, 100, GUIOT_IMAGE, NULL);
	InitImage(&MainScreen.WgTrackFmt);

	GUI_InitObject(&MainScreen.WgPlayMode, 90, 0, 30, 19, 100, GUIOT_IMAGE, NULL);
	InitImage(&MainScreen.WgPlayMode);

	GUI_InitObject(&MainScreen.StrBatLevel, 193, 220, 0, 0, 100, GUIOT_STRING, NULL);
	GUI_InitString(&MainScreen.StrBatLevel, &Font16, 200, 240);

	/* PICTURES */
	GUI_InitObject(&MainScreen.ImgTrackPicture, 0, 0, 240, 240, 100, GUIOT_IMAGE, NULL);
	InitImage(&MainScreen.ImgTrackPicture);
}


void qc_GUI::GUI_InitObject (qsGUI_Object_t *obj,
							 ui16 x, ui16 y, ui16 width, ui16 height,
							 ui8 opacity, qeGUI_ObjectType_t type,
							 void *customFunc)
{
	obj->visible = false;
	obj->needUpdate = false;

	obj->x = x;
	obj->y = y;
	obj->width = width;
	obj->height = height;
	obj->opacity = opacity;
	obj->type = type;

	obj->customFunc = customFunc;
}


void qc_GUI::GUI_ShowObject (qsGUI_Object_t *obj)
{
	obj->visible = true;
	obj->needUpdate = true;
}


void qc_GUI::GUI_HideObject (qsGUI_Object_t *obj)
{
	obj->visible = false;
	obj->needUpdate = true;
}


void qc_GUI::GUI_MoveObject (qsGUI_Object_t *obj, ui16 newX, ui16 newY)
{
	obj->x = newX;
	obj->y = newY;

	obj->needUpdate = true;
}


void qc_GUI::GUI_HandleObjects (void)
{
	qsGUI_Object_t *obj = &MainScreen.WgSampleRate;

	for (ui8 i = 0; i < MainScreen.objectNumber; i++)
	{
		if (obj[i].needUpdate)
		{
//			if (obj[i].customFunc)
//				obj[i].customFunc();

			GUI_RedrawObject(&obj[i]);

			obj[i].needUpdate = false;
		}
	}
}


void qc_GUI::GUI_RedrawObject (qsGUI_Object_t *obj)
{
	FillBuff(obj->x, obj->y, obj->width, obj->height, obj->buff);
}


void qc_GUI::GUI_InitString (qsGUI_Object_t *obj, tFont *font, ui16 xEnd, ui16 yEnd)
{
	qsGUI_StringParams_t params = { 0 };

	obj->params = qmCreate(qsGUI_StringParams_t);

	params.font = font;

	params.scrollPhase = 0;
	params.xEnd = xEnd;
	params.yEnd = yEnd;

	*((qsGUI_StringParams_t*)obj->params) = params;
}


void qc_GUI::GUI_RenderString (qsGUI_Object_t *obj, char *str, ui16 color, ui16 bgColor, bool background)
{
	qsGUI_StringParams_t *params = ((qsGUI_StringParams_t*)(obj->params));

	params->length = strlen(str);

	params->color = color;
	params->bgColor = bgColor;
	params->background = background;

	((qsGUI_StringParams_t*)obj->params)->str = qmCreateArr(char, params->length);
	strcpy(((qsGUI_StringParams_t*)obj->params)->str, str);

    ui16 totalWidth = 0;
    char *s = params->str;

    while (*s)
    {
        ui32 code = UTF8MergeCode(&s);
        const tImage *img = FindCharImage(params->font, code);
        totalWidth += img ? img->width : 4;
    }

    ui16 h = params->font->chars[0].image->height;

    obj->width = totalWidth;
    obj->height = h;

    if (obj->buff)
    	obj->buff = (ui16*)reallocarray(obj->buff, totalWidth * h * sizeof(ui16), sizeof(ui16));

    else
    	obj->buff = (ui16*)qmCreateArr(ui16, totalWidth * h * sizeof(ui16));

    if (params->background)
    	memset(obj->buff, params->bgColor, totalWidth * h);

    ui16 cursorX = 0;
    s = params->str;

    while (*s)
    {
        ui32 code = UTF8MergeCode(&s);
        const tImage *img = FindCharImage(params->font, code);

        if (img)
        {
            DrawGlyphToBuf(obj, img);
            cursorX += img->width;
        }

        else
            cursorX += 4;
    }

    obj->needUpdate = true;
}


void qc_GUI::InitImage (qsGUI_Object_t *obj)
{
	obj->buff = qmCreateArr(ui16, obj->height * obj->width * sizeof(ui16));
}


void qc_GUI::SetImage (qsGUI_Object_t *obj, ui16 *buffer)
{
	obj->buff = buffer;
	obj->needUpdate = true;
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
//	if 		(Metadata.sampleRate == 32000) SetImage (objects[4], image_data_SF32);
//	else
		if (Metadata.sampleRate == 44100)  ST7789_DrawPartofImageInverted(0, 0, 30, 19, image_data_SF44);
	else if (Metadata.sampleRate == 48000)  ST7789_DrawPartofImageInverted(0, 0, 30, 19, image_data_SF48);
	else if (Metadata.sampleRate == 96000)  ST7789_DrawPartofImageInverted(0, 0, 30, 19, image_data_SF96);
	else if (Metadata.sampleRate == 192000) ST7789_DrawPartofImageInverted(0, 0, 30, 19, image_data_SF192);

	if 		(Metadata.bitRate == 12)  	ST7789_DrawPartofImageInverted(34, 0, 30, 19, image_data_BR12);
	else if (Metadata.bitRate == 16)  	ST7789_DrawPartofImageInverted(34, 0, 30, 19, image_data_BR16);
	else if (Metadata.bitRate == 24)  	ST7789_DrawPartofImageInverted(34, 0, 30, 19, image_data_BR24);
	else if (Metadata.bitRate == 32) 	ST7789_DrawPartofImageInverted(34, 0, 30, 19, image_data_BR32);

//	char batlev [5];
//	batteryLevel *= 0.02454;
//	sprintf(batlev, "%02d%%", (int)batteryLevel);
//	ST7789_WriteString(190, 2, batlev, &Font16, BLACK, WHITE);
//	ST7789_WriteString(193, 220, "Pause", &Font16, WHITE, BLACK);
}




void qc_GUI::PrintTrackInfo (void)
{
	GUI_RenderString(&MainScreen.StrTrackTitle, Metadata.title, WHITE, BLACK, true);
	GUI_RenderString(&MainScreen.StrTrackArtist, Metadata.artist, WHITE, BLACK, true);
//	ST7789_DrawFilledRectangle(10, 195, 195, 240 - 195, BLACK);
//	ST7789_WriteString(10, 195, Metadata.title, &Font20, WHITE, BLACK);
//	ST7789_WriteString(10, 220, Metadata.artist, &Font16, WHITE, BLACK);
}

void qc_GUI::PrintRender (void)
{
	donothing_;
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
		GUI_RenderString(&MainScreen.StrTrackDrt, timeStr, WHITE, BLACK, true);
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
	if (selected == 0)
	{
		strcpy(menu.highlayer[0], "Font");
		strcpy(menu.highlayer[1], "Folders");
		strcpy(menu.highlayer[2], "Language");
		strcpy(menu.highlayer[3], "Sleep mode");
		strcpy(menu.highlayer[4], "About/Help");
		strcpy(menu.lang[2],"Back");
		strcpy(about,"MicroPlayer project GitHub: qsivey, Nik125Y Telegram: @qsivey, @Nik125Y Email: qsivey@gmail.com, topnikm@gmail.com");
	}

	if (selected == 1)
	{
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


void qc_GUI::DrawMenu(ui8 selected)
{
	ST7789_FillColor(BLACK);

	for (uint8_t i = 0; i < MAX_COMPONENTS; i++)
	{
		uint16_t color = (i == selected) ? YELLOW : WHITE;
//		ST7789_WriteString(5, i * 23, menu.highlayer[i], &Font20, color, BLACK);
	}
}


void qc_GUI::DrawFolderMenu(ui8 selected)
{
	ST7789_FillColor(BLACK);

	for (uint8_t i = 0; i < folderCount; i++)
	{
		uint16_t color = (i == selected) ? YELLOW : WHITE;
//		ST7789_WriteString(5, i * 23, folders[i], &Font20, color, BLACK);
	}
}


void qc_GUI::Language(ui8 selected)
{
	ST7789_FillColor(BLACK);
	for (uint8_t i = 0; i < 3; i++) {
		uint16_t color = (i == (selected)) ? YELLOW : WHITE;
//		ST7789_WriteString(5, i * 23, menu.lang[i], &Font20, color, BLACK);
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
//	ST7789_FillColor(BLACK);
//	ST7789_WriteString(0, 4, about, &Font20, WHITE, BLACK);
//	ST7789_WriteString(5, 200, menu.lang[2], &Font20, YELLOW, BLACK);
}

/* -------------------------------------------------
 * Helpers
 */
ui16 qc_GUI::Mix565 (ui16 color, ui16 bgColor, ui8 alpha)
{
	ui8 r = (color >> 11) & 0x1F;
	ui8 g = (color >> 5) & 0x3F;
	ui8 b = color & 0x1F;

	ui8 bgr = (bgColor >> 11) & 0x1F;
	ui8 bgg = (bgColor >> 5) & 0x3F;
	ui8 bgb = bgColor & 0x1F;

	r = ((r * alpha) + (bgr * (63 - alpha))) / 63;
	g = ((g * alpha) + (bgg * (63 - alpha))) / 63;
	b = ((b * alpha) + (bgb * (63 - alpha))) / 63;

	return (r << 11) | (g << 5) | b;
}


ui32 qc_GUI::UTF8MergeCode (char **ptr)
{
    const ui8 *s = (const ui8 *)(*ptr);

    ui32 code = 0;

    if (s[0] < 0x80)
    {
        // English
        code = s[0];
        *ptr += 1;
    }
    else if ((s[0] & 0xE0) == 0xC0)
    {
        // Russia
        code = (s[0] << 8) | s[1];
        *ptr += 2;
    }
    else if ((s[0] & 0xF0) == 0xE0)
    {
        //  №
        code = (s[0] << 16) | (s[1] << 8) | s[2];
        *ptr += 3;
    }
    else
    {
        code = '?';
        *ptr += 1;
    }

    return code;
}


const tChar *qc_GUI::FindChar (const tFont *font, int32_t code)
{
    for (int i = 0; i < font->length; ++i)
        if (font->chars[i].code == code) return &font->chars[i];
    return NULL;
}


const tImage *qc_GUI::FindCharImage (const tFont *font, long int code)
{
    for (int i = 0; i < font->length; ++i)
        if (font->chars[i].code == code) return font->chars[i].image;
    return NULL;
}


inline ui16 qc_GUI::RGB565 (ui8 r, ui8 g, ui8 b)
{
    return ( ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3) );
}


void qc_GUI::DrawGlyphToBuf (qsGUI_Object_t *obj, const tImage *img)
{
	qsGUI_StringParams_t *params = ((qsGUI_StringParams_t*)(obj->params));

    ui16 gw = obj->width;
    ui16 gh = obj->height;
    const ui8 *src = img->data;
    ui16 pixelIndex = 0;

    for (int yy = 0; yy < gh; yy++)
    {
        for (int xx = 0; xx < gw; xx++)
        {
            ui16 byteIndex = pixelIndex >> 1;
            ui8 byte = src[byteIndex];
            ui8 val = (pixelIndex & 1) ? (byte & 0x0F) : (byte >> 4);
            ui8 alpha = val ? ((val + 1) * 4) - 1 : 0;
            pixelIndex++;

            if (!(params->background) && alpha == 0xF) continue;

            obj->buff[yy * obj->width + xx] = Mix565(params->color, params->bgColor, 63 - alpha);
        }
    }
}


