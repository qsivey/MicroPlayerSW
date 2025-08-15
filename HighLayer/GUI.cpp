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

/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   Class Methods
 */


size_t qc_GUI::InputJPEG (JDEC* jd, uint8_t* buff, size_t len)
{
	UINT bytesRead = 0;
	qc_GUI* self = (qc_GUI*)jd->device;

    if (!buff)
    {
    	buff = qmCreateArr(ui8, len);
		f_read(&self->JPEG_Temp->file, buff, len, &bytesRead);
		qmFree(buff);

		return len;
    }

	if (f_read(&self->JPEG_Temp->file, buff, len, &bytesRead) != FR_OK)
		return 0;

    return bytesRead;
}


int qc_GUI::OutputJPEG (JDEC *jd, void *bitmap, JRECT *rect)
{
	qc_GUI *self = (qc_GUI*)jd->device;

	self->ScaleJPEG((ui16*)bitmap, rect);

	return 1;
}


int qc_GUI::ScaleJPEG (void *bitmap, JRECT *rect)
{
	ui16 *src = (ui16 *)bitmap;

	ui16 srcWidth = rect->right - rect->left + 1;
	ui16 srcHeight = rect->bottom - rect->top + 1;

	float scaleX = (float)qcfgPICTURE_RESOLUTION / JPEG_Temp->jd.width;
	float scaleY = (float)qcfgPICTURE_RESOLUTION / JPEG_Temp->jd.height;

	for (ui16 sy = 0; sy < srcHeight; sy++)
	{
		ui16 srcY = rect->top + sy;
		float fy = srcY * scaleY;
		ui16 dy = (ui16)fy;

		if (dy >= qcfgPICTURE_RESOLUTION)
			continue;

		for (ui16 sx = 0; sx < srcWidth; sx++)
		{
			ui16 srcX = rect->left + sx;
			float fx = srcX * scaleX;
			ui16 dx = (ui16)fx;

			if (dx >= qcfgPICTURE_RESOLUTION)
				continue;

			ui16 color = src[sy * srcWidth + sx];
			displayBuffer[dy * qcfgPICTURE_RESOLUTION + dx] = __REV16(color);
		}
	}

	return 1;
}


bool qc_GUI::RenderJPEG (void)
{
	JPEG_Temp->jd.device = (void*)this;

	JRESULT res = jd_prepare(&JPEG_Temp->jd, InputJPEG, JPEG_Temp->tjdBuff, qcfgTJD_BUFF_SIZE, this);

	if (res == JDR_OK)
	{
		flagOutOfImage = false;
		jd_decomp(&JPEG_Temp->jd, OutputJPEG, 0);
	}
	else
	{
		flagOutOfImage = true;
		return false;
	}

	return true;
}


void qc_GUI::MP3MetaInfo(const TCHAR* path){
	FIL file;
	if (f_open(&file, path, FA_READ) == FR_OK) {
		if (parse_mp3_metadata(&file, &mp3_meta) == FR_OK) {
			mp3_meta.flag = true;
		}
		f_close(&file);
	}
}


void qc_GUI::FlacMetaInfo(const TCHAR* path){
	FIL file;
	if (f_open(&file, path, FA_READ) == FR_OK) {
		if (parse_flac_metadata(&file, &flac_meta) == FR_OK) {
			flac_meta.flag = true;
		}
		f_close(&file);
	}
}


void qc_GUI::WAVMetaInfo(const TCHAR* path){
	FIL file;
	if (f_open(&file, path, FA_READ) == FR_OK) {
		if (parse_wav_metadata(&file, &wav_meta) == FR_OK) {
			wav_meta.flag = true;
		}
		f_close(&file);
	}
}


void qc_GUI::CacheDisplay (const char *filename)
{
    FIL file;
    FRESULT res;
    UINT written;

    qsBMP_Header_t header;
    qsBMP_InfoHeader_t info;

    uint32_t row_size = qcfgPICTURE_RESOLUTION * 2;
    uint32_t image_size = row_size * qcfgPICTURE_RESOLUTION;
    uint32_t file_size = sizeof(qsBMP_Header_t) + sizeof(qsBMP_InfoHeader_t) + image_size;

    header.bfType = 0x4D42; // 'BM'
    header.bfSize = file_size;
    header.bfReserved1 = 0;
    header.bfReserved2 = 0;
    header.bfOffBits = sizeof(qsBMP_Header_t) + sizeof(qsBMP_InfoHeader_t);

    info.biSize = sizeof(qsBMP_InfoHeader_t);
    info.biWidth = qcfgPICTURE_RESOLUTION;
    info.biHeight = -qcfgPICTURE_RESOLUTION;
    info.biPlanes = 1;
    info.biBitCount = 16;  // RGB565
    info.biCompression = 3; // BI_BITFIELDS
    info.biSizeImage = image_size;
    info.biXPelsPerMeter = 0;
    info.biYPelsPerMeter = 0;
    info.biClrUsed = 0;
    info.biClrImportant = 0;

    const uint32_t masks [3] = { 0xF800, 0x07E0, 0x001F };

    f_chdir(qcfgCACHE_IMG_PATH);

    res = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) return;

    f_write(&file, &header, sizeof(header), &written);
    f_write(&file, &info, sizeof(info), &written);
    f_write(&file, masks, sizeof(masks), &written);

    // --- Запись пикселей ---
    for (int y = 0; y < qcfgPICTURE_RESOLUTION; y++)
    {
        ui16 *row = &displayBuffer[qcfgPICTURE_RESOLUTION * y];

        for (ui16 i = 0; i < qcfgPICTURE_RESOLUTION; i++)
		{
        	row[i] = __REV16(row[i]);
		}

        f_write(&file, row, row_size, &written);
    }

    f_close(&file);
}


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
	if (flac_meta.sampleRate == 44100 ||
		mp3_meta.sampleRate  == 44100 ||
		wav_meta.sample_rate == 44100)
		ST7789_DrawPartofImage(0, 0, 30, 19, image_data_SF44);

	if (flac_meta.sampleRate == 48000 ||
		mp3_meta.sampleRate  == 48000 ||
		wav_meta.sample_rate == 48000)
		ST7789_DrawPartofImage(0, 0, 30, 19, image_data_SF48);

	if (flac_meta.sampleRate == 192000 ||
		mp3_meta.sampleRate  == 192000 ||
		wav_meta.sample_rate == 192000)
		ST7789_DrawPartofImage(0, 0, 30, 19, image_data_SF192);

	if (flac_meta.sampleRate == 32000 ||
		mp3_meta.sampleRate  == 32000 ||
		wav_meta.sample_rate == 32000)
		ST7789_DrawPartofImage(0, 0, 30, 19, image_data_SF32);

	char batlev[16];
	batteryLevel *= 0.02454;
	sprintf(batlev, "%02d%", (int)batteryLevel);
	ST7789_WriteString(190, 2, batlev, &Font16, WHITE, BLACK);

	ST7789_WriteString(193, 220, "pause", &Font16, WHITE, BLACK);
}


void qc_GUI::PrintTrackInfo (void)
{
	ST7789_FillColor(BLACK);

	/* flac */
	if (flac_meta.flag) ST7789_WriteString(10, 195, flac_meta.title, &Font20, WHITE, BLACK);
	if (flac_meta.flag) ST7789_WriteString(10, 220, flac_meta.artist, &Font16, WHITE, BLACK);

	/* mp3 */
	if (mp3_meta.flag) ST7789_WriteString(10, 195, mp3_meta.title, &Font20, WHITE, BLACK);
	if (mp3_meta.flag) ST7789_WriteString(10, 220, mp3_meta.artist, &Font16, WHITE, BLACK);

	/* wav */
	if (wav_meta.flag) ST7789_WriteString(10, 195, wav_meta.title, &Font20, WHITE, BLACK);
	if (wav_meta.flag) ST7789_WriteString(10, 220, wav_meta.artist, &Font16, WHITE, BLACK);
}


void qc_GUI::PrintTrackPicture(const TCHAR* path, char original_path)
{
	char cachePathBuf [10] = { 0 };
	JPEG_Temp = qmCreate(qsJPEG_t);

	if (f_open(&JPEG_Temp->file, path, FA_READ) == FR_OK) {

	        RenderJPEG();

	        if (!flagOutOfImage) ST7789_DrawImage(0, 0, 240, 240, displayBuffer);

			CacheDisplay(cachePathBuf);

			f_chdir(&original_path);

	    f_close(&JPEG_Temp->file);
	}
	qmFree(JPEG_Temp);
}


void qc_GUI::PrintTrackTime (void)
{
	float duration_sec = 0;
	ui32 seconds = 0, minutes = 0;
	char timeStr[16];

	if (flac_meta.flag || mp3_meta.flag || wav_meta.flag) {

		if (flac_meta.flag) duration_sec = (float)flac_meta.totalSamples / flac_meta.sampleRate;
		if (mp3_meta.flag)  duration_sec = mp3_meta.duration_sec; //not valid
		if (wav_meta.flag)  duration_sec = (float)wav_meta.data_size    / wav_meta.byte_rate; //not valid

		startTime = qmGetTick();
		seconds = (ui32)duration_sec;
		minutes = seconds / 60;
		seconds %= 60;

		sprintf(timeStr, "%02d:%02d", minutes, seconds);
		ST7789_WriteString(193, 220, timeStr, &Font16, WHITE, BLACK);
	}
}


void qc_GUI::PrintTrackCurrentTime(void)
{
	ui32 seconds = 0, minutes = 0;
	char timeStr[16];
	ui32 curTime = (qmGetTick() - startTime) / 1000;

	seconds = curTime;
	minutes = seconds / 60;
	seconds %= 60;

	sprintf(timeStr, "%02d:%02d", minutes, seconds);
	ST7789_WriteString(185, 195, timeStr, &Font20, WHITE, BLACK);
}


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

