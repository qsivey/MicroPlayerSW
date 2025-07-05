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

void qc_GUI::RenderJPEG (void)
{
	JPEG_Temp->jd.device = (void*)this;

	JRESULT res = jd_prepare(&JPEG_Temp->jd, InputJPEG, JPEG_Temp->tjdBuff, qcfgTJD_BUFF_SIZE, this);

	if (res == JDR_OK)
		jd_decomp(&JPEG_Temp->jd, OutputJPEG, 0);
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

    const uint32_t masks[3] = { 0xF800, 0x07E0, 0x001F };

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

    f_chmod(filename, AM_HID, AM_HID);

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

}
void qc_GUI::DrawDurationBar (void)
{

}
void qc_GUI::PrintTrackInfo (void)
{

}
