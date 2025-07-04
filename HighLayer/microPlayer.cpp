/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#include 	"microPlayer.h"

#include	"dr_mp3.h"
#include	"dr_flac.h"
#include	"dr_wav.h"

#include    "st7789.h"

/* Offset from the current track name end symbol */
#define		qsbCurTrNameEndOfs(ofs)	(fatFS.playlistTrackNames[fatFS.playlistTrackNamePointers[trackNumber + 1] - 1 - ofs])


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														  Global Objects
 */
qc_uPlayer uPlayer;


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   Class Methods
 */
uPlayerStatus_t qc_uPlayer::OpenMP3 (void)
{
	track = qmCreate(drmp3);

	drmp3_init_file((drmp3*)track, trackPath, NULL);

	PCM5142_SetSampleRate(((drmp3*)track)->sampleRate);
	PCM5142_SetBitRate(16);

	PCM_Buffer = qmCreateArr(i16, qcfgPCM_BUFFER_SIZE * sizeof(i16));

	bufferOffset = BUFFER_OFFSET_FULL;

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::OpenFLAC (void)
{
	track = drflac_open_file(trackPath, NULL);

	PCM5142_SetSampleRate(((drflac*)track)->sampleRate);
	PCM5142_SetBitRate(((drflac*)track)->bitsPerSample);

	if (((drflac*)track)->bitsPerSample <= 16)
	{
		ReadFrames = &qc_uPlayer::ReadFLAC_16;
		PCM_Buffer = qmCreateArr(drflac_int16, qcfgPCM_BUFFER_SIZE * sizeof(drflac_int16));
	}

	else
	{
		ReadFrames = &qc_uPlayer::ReadFLAC_32;
		PCM_Buffer = qmCreateArr(drflac_int32, qcfgPCM_BUFFER_SIZE * sizeof(drflac_int32));
	}

	bufferOffset = BUFFER_OFFSET_FULL;

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::OpenWAV (void)
{
	track = qmCreate(drwav);

	drwav_init_file((drwav*)track, trackPath, NULL);

	PCM5142_SetSampleRate(((drwav*)track)->sampleRate);
	PCM5142_SetBitRate(((drwav*)track)->bitsPerSample);

	if (((drwav*)track)->bitsPerSample <= 16)
	{
		ReadFrames = &qc_uPlayer::ReadWAV_16;
		PCM_Buffer = qmCreateArr(drwav_int16, qcfgPCM_BUFFER_SIZE * sizeof(drwav_int16));
	}

	else
	{
		ReadFrames = &qc_uPlayer::ReadWAV_32;
		PCM_Buffer = qmCreateArr(drwav_int32, qcfgPCM_BUFFER_SIZE * sizeof(drwav_int32));
	}

	bufferOffset = BUFFER_OFFSET_FULL;

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::ReadMP3 (void)
{
	if (bufferOffset == BUFFER_OFFSET_FULL)
		framesRead = drmp3_read_pcm_frames_s16((drmp3*)track, qcfgPCM_BUFFER_SIZE / 4, (drmp3_int16*)PCM_Buffer);

	else if (bufferOffset == BUFFER_OFFSET_HALF)
		framesRead = drmp3_read_pcm_frames_s16((drmp3*)track, qcfgPCM_BUFFER_SIZE / 4,
				(drmp3_int16*)(((drmp3_int16*)PCM_Buffer) + (qcfgPCM_BUFFER_SIZE / 2)));

	assign_(bufferOffset, BUFFER_OFFSET_NONE);

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::ReadFLAC_16 (void)
{
	if (bufferOffset == BUFFER_OFFSET_FULL)
		framesRead = drflac_read_pcm_frames_s16((drflac*)track, qcfgPCM_BUFFER_SIZE / 4, (drflac_int16*)PCM_Buffer);

	else if (bufferOffset == BUFFER_OFFSET_HALF)
		framesRead = drflac_read_pcm_frames_s16((drflac*)track, qcfgPCM_BUFFER_SIZE / 4,
				(drflac_int16*)(((drflac_int16*)PCM_Buffer) + (qcfgPCM_BUFFER_SIZE / 2)));

	assign_(bufferOffset, BUFFER_OFFSET_NONE);

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::ReadFLAC_32 (void)
{
	if (bufferOffset == BUFFER_OFFSET_FULL)
		framesRead = drflac_read_pcm_frames_s32((drflac*)track, qcfgPCM_BUFFER_SIZE / 4, (drflac_int32*)PCM_Buffer);

	else if (bufferOffset == BUFFER_OFFSET_HALF)
		framesRead = drflac_read_pcm_frames_s32((drflac*)track, qcfgPCM_BUFFER_SIZE / 4,
				(drflac_int32*)(((drflac_int32*)PCM_Buffer) + (qcfgPCM_BUFFER_SIZE / 2)));

	assign_(bufferOffset, BUFFER_OFFSET_NONE);

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::ReadWAV_16 (void)
{
	if (bufferOffset == BUFFER_OFFSET_FULL)
		framesRead = drwav_read_pcm_frames_s16((drwav*)track, qcfgPCM_BUFFER_SIZE / 4, (drwav_int16*)PCM_Buffer);

	else if (bufferOffset == BUFFER_OFFSET_HALF)
		framesRead = drwav_read_pcm_frames_s16((drwav*)track, qcfgPCM_BUFFER_SIZE / 4,
				(drwav_int16*)(((drwav_int16*)PCM_Buffer) + (qcfgPCM_BUFFER_SIZE / 2)));

	assign_(bufferOffset, BUFFER_OFFSET_NONE);

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::ReadWAV_32 (void)
{
	if (bufferOffset == BUFFER_OFFSET_FULL)
		framesRead = drwav_read_pcm_frames_s32((drwav*)track, qcfgPCM_BUFFER_SIZE / 4, (drwav_int32*)PCM_Buffer);

	else if (bufferOffset == BUFFER_OFFSET_HALF)
		framesRead = drwav_read_pcm_frames_s32((drwav*)track, qcfgPCM_BUFFER_SIZE / 4,
				(drwav_int32*)(((drwav_int32*)PCM_Buffer) + (qcfgPCM_BUFFER_SIZE / 2)));

	assign_(bufferOffset, BUFFER_OFFSET_NONE);

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::CloseMP3 (void)
{
	drmp3_uninit((drmp3*)track);

	qmDestroy(PCM_Buffer);
	qmDestroy(track);

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::CloseFLAC (void)
{
	drflac_close((drflac*)track);

	qmDestroy(PCM_Buffer);
	qmDestroy(track);

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::CloseWAV (void)
{
	drwav_uninit((drwav*)track);

	qmDestroy(PCM_Buffer);
	qmDestroy(track);

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::Init (void)
{
	HAL_Init();

	SystemClockConfig();
	PeriphCommonClockConfig();

	/* Enable all GPIOs */
	AllGPIO_ClockEnable();

	/* Display Init */
	ST7789_Init();

	/* Analog channels init */
//	ADC_Init();

	/* All buttons init */
	ButtonsInit();

	/* Mount file system */
	extern Diskio_drvTypeDef SDMMC_Driver;

	FATFS_LinkDriver(&SDMMC_Driver, fatFS.SD_Path);

	if (f_mount(&fatFS.fs, (TCHAR const*)fatFS.SD_Path, 0) != FR_OK)
		return uPL_ERROR;

	/* PCM5142 init */
	PCM5142_Init();

	/* Service variable init */
	currentCodec = uPLFC_NONE;
	trackNumber = 0;

	event = uPL_EVENT_NONE;
	backlightTimer = qmGetTick();

	playMode = (uPlayerPlayMode_t)(uPL_MODE_NORMAL | uPL_MODE_REPEAT_PLAYLIST);

	state = uPL_STOP;

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::InitFolder (const char *folder)
{
	ui16 fileNameLength = 0;
	ui32 totalFileNameLength = 0;

	totalTrackNumber = 0;

	FRESULT res = f_chdir(folder);

	/* Enter into a folder */
	res = f_findfirst(&fatFS.dir, &fatFS.fno, folder, "*");

	/* Count total file name length */
	while (res == FR_OK && fatFS.fno.fname[0])
	{
		if (!(fatFS.fno.fattrib & AM_DIR))  // Skip the folders
		{
			totalTrackNumber++;
			totalFileNameLength += strlen(fatFS.fno.fname);

			res = f_findnext(&fatFS.dir, &fatFS.fno);
		}
	}

	fatFS.playlistTrackNames = qmCreateArr(char, totalFileNameLength + totalTrackNumber);
	fatFS.playlistTrackNamePointers = qmCreateArr(ui16, totalTrackNumber * 2 + 2);

	totalFileNameLength = 0;  // For now this is the cursor

	res = f_findfirst(&fatFS.dir, &fatFS.fno, folder, "*");

	/* Write file names and assign the pointers */
	ui32 i = 0;

	for (; (res == FR_OK) && (fatFS.fno.fname[0]); i++)
	{
		if (!(fatFS.fno.fattrib & AM_DIR))  // Skip the folders
		{
			fileNameLength = strlen(fatFS.fno.fname) + 1;

			fatFS.playlistTrackNamePointers[i] = totalFileNameLength;

			memcpy(&fatFS.playlistTrackNames[totalFileNameLength], fatFS.fno.fname, fileNameLength);
			totalFileNameLength += fileNameLength;

			res = f_findnext(&fatFS.dir, &fatFS.fno);
		}
	}

	fatFS.playlistTrackNamePointers[i] = totalFileNameLength;

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::InitCodec (void)
{
	/* Load current track from playlist */
	trackPath = &fatFS.playlistTrackNames[fatFS.playlistTrackNamePointers[trackNumber]];

	/* Check current audio file */
	uPlayerFileCodec_t fileCodec;

	// todo add other formats
	if (((qsbCurTrNameEndOfs(3) == 'm') && (qsbCurTrNameEndOfs(2) == 'p') && (qsbCurTrNameEndOfs(1) == '3')) ||
		((qsbCurTrNameEndOfs(3) == 'M') && (qsbCurTrNameEndOfs(2) == 'P') && (qsbCurTrNameEndOfs(1) == '3')))
		fileCodec = uPLFC_MP3;

	/* .flac */
	else if (((qsbCurTrNameEndOfs(4) == 'f') && (qsbCurTrNameEndOfs(3) == 'l') && (qsbCurTrNameEndOfs(2) == 'a') && (qsbCurTrNameEndOfs(1) == 'c')) ||
		((qsbCurTrNameEndOfs(4) == 'F') && (qsbCurTrNameEndOfs(3) == 'L') && (qsbCurTrNameEndOfs(2) == 'A') && (qsbCurTrNameEndOfs(1) == 'C')))
		fileCodec = uPLFC_FLAC;

	/* .ogg */
	else if (((qsbCurTrNameEndOfs(3) == 'o') && (qsbCurTrNameEndOfs(2) == 'g') && (qsbCurTrNameEndOfs(1) == 'g')) ||
		((qsbCurTrNameEndOfs(3) == 'O') && (qsbCurTrNameEndOfs(2) == 'G') && (qsbCurTrNameEndOfs(1) == 'G')))
		fileCodec = uPLFC_OGG;

	/* .wav */
	else if (((qsbCurTrNameEndOfs(3) == 'w') && (qsbCurTrNameEndOfs(2) == 'a') && (qsbCurTrNameEndOfs(1) == 'v')) ||
		((qsbCurTrNameEndOfs(3) == 'W') && (qsbCurTrNameEndOfs(2) == 'A') && (qsbCurTrNameEndOfs(1) == 'V')))
		fileCodec = uPLFC_WAV;

	/* .wv todo??? */
	else if (((qsbCurTrNameEndOfs(2) == 'w') && (qsbCurTrNameEndOfs(1) == 'v')) ||
		((qsbCurTrNameEndOfs(2) == 'W') && (qsbCurTrNameEndOfs(1) == 'V')))
		fileCodec = uPLFC_WAV;

	else
	{
		SetEvent(uPL_EVENT_TRACK_NEXT);
		status = uPL_ERROR;
		return uPL_ERROR;  // Not supported format
	}

	if (currentCodec != fileCodec)
	{
		switch (fileCodec)
		{
			case uPLFC_MP3 :
				OpenTrack = &qc_uPlayer::OpenMP3;
				CloseTrack = &qc_uPlayer::CloseMP3;
				ReadFrames = &qc_uPlayer::ReadMP3;
				break;

			case uPLFC_FLAC :
			case uPLFC_OGG :
				OpenTrack = &qc_uPlayer::OpenFLAC;
				CloseTrack = &qc_uPlayer::CloseFLAC;
				break;

			case uPLFC_WAV :
			{
				OpenTrack = &qc_uPlayer::OpenWAV;
				CloseTrack = &qc_uPlayer::CloseWAV;
				break;
			}

			default :
				break;
		}

		currentCodec = fileCodec;
	}

	/* Preparing */
	qmCall(OpenTrack);

	qmCall(ReadFrames);
	bufferOffset = BUFFER_OFFSET_HALF;

	/* Unmute */
	PCM5142_Unmute();

	/* Start DMA */
	HAL_I2S_Transmit_DMA(&DAC_I2S_HANDLE, (ui16*)PCM_Buffer, qcfgPCM_BUFFER_SIZE);

	state = uPL_PLAY;
	status = uPL_OK;

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::DeinitCodec (void)
{
	/* Mute */
	PCM5142_Mute();
	qmDelayMs(20);

	/* Stop DMA */
	HAL_I2S_DMAStop(&DAC_I2S_HANDLE);

	/* Close current track */
	qmCall(CloseTrack);

	return uPL_OK;
}


uPlayerStatus_t	qc_uPlayer::EventHandler (void)
{
	if (event == uPL_EVENT_NONE)
		return uPL_OK;

	backlightTimer = qmGetTick();

	switch (event)
	{
		case uPL_EVENT_VOLUME_DOWN :
			VolumeDown();
			ResetEvent(uPL_EVENT_VOLUME_DOWN);
			break;

		case uPL_EVENT_VOLUME_UP :
			VolumeUp();
			ResetEvent(uPL_EVENT_VOLUME_UP);
			break;

		case uPL_EVENT_TRACK_BACK :
			Back();
			ResetEvent(uPL_EVENT_TRACK_BACK);
			break;

		case uPL_EVENT_TRACK_NEXT :
			Next();
			ResetEvent(uPL_EVENT_TRACK_NEXT);
			break;

		case uPL_EVENT_TRACK_PAUSE_PLAY :
			if (state == uPL_STOP)
				InitCodec();

			else if (state == uPL_PAUSE)
			{
				PCM5142_Unmute();

				HAL_I2S_DMAResume(&DAC_I2S_HANDLE);

				state = uPL_PLAY;
			}

			else if (state == uPL_PLAY)
			{
				Pause();

				state = uPL_PAUSE;
			}

			ResetEvent(uPL_EVENT_TRACK_PAUSE_PLAY);

			break;

		default :
			break;
	}

	return uPL_OK;
}


uPlayerStatus_t	qc_uPlayer::VolumeUp (void)
{
	if (volume < qcfgMAX_VOLUME)
	{
		volume++;
		PCM5142_SetVolume(volume);
	}

	else
	{
		if (amplitudeBoost == false)
		{
			PCM5142_Write(0, 1);
			PCM5142_Write(7, 0x11);  // Boost amplitude
			PCM5142_Write(0, 0);

			amplitudeBoost = true;
		}

		volume = qcfgMAX_VOLUME;
	}

//	ShowVolumeBar(volume);

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::VolumeDown (void)
{
	if (volume > qcfgMIN_VOLUME)
	{
		if (amplitudeBoost)
		{
			PCM5142_Write(0, 1);
			PCM5142_Write(7, 0);  // Boost amplitude off
			PCM5142_Write(0, 0);

			amplitudeBoost = false;
		}

		volume--;
		PCM5142_SetVolume(volume);
	}

	else
		volume = qcfgMIN_VOLUME;

//	ShowVolumeBar(volume);

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::Back (void)
{
	DeinitCodec();

	if (trackNumber)
		/* Go to the previous track */
		trackNumber--;

	else
		/* Go to the end of playlist */
		trackNumber = totalTrackNumber - 1;

	return InitCodec();
}


uPlayerStatus_t qc_uPlayer::Next (void)
{
	if (status != uPL_ERROR)
		DeinitCodec();

	if ((playMode & uPL_MODE_REPEAT_TRACK) && (!GetEvent(uPL_EVENT_TRACK_NEXT)))
	/* Track is over in one track repeat mode */
	{
		/* Repeat it again */
		donothing_;
	}

	else
	/* Load next track */
	{
		trackNumber++;

		if (trackNumber == totalTrackNumber)
		/* End of playlist */
		{
			if (!(playMode & uPL_MODE_REPEAT_PLAYLIST))
			/* Return to the start and stop */
				Stop();

			/* else: return to the start and playing again */
			trackNumber = 0;
		}
	}

	return InitCodec();
}


uPlayerStatus_t qc_uPlayer::Stop (void)
{
	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::Pause (void)
{
	PCM5142_Mute();
	qmDelayMs(20);

	HAL_I2S_DMAPause(&uPlayer.DAC_I2S_HANDLE);

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::Play (void)
{
	ui32 cycleTemp = qmGetTick();

	/* Decode next frame */
	qmCall(ReadFrames);

	cycle = qmGetTick() - cycleTemp;

	if (!framesRead)
		Next();

	return uPL_OK;
}


typedef struct
{
	FIL* file;
	uint32_t remaining;

}	jpeg_source_t;


UINT tjd_input(JDEC* jd, uint8_t* buf, UINT len)
{
    jpeg_source_t* src = (jpeg_source_t*)jd->device;
    UINT br;
    if (len > src->remaining) len = src->remaining;

    if (!buf)
    {
     buf = qmCreateArr(ui8, len);
     f_read(src->file, buf, len, &br);
     qmFree(buf);

     return len;
    }

    if (f_read(src->file, buf, len, &br) != FR_OK)
     return 0;

    src->remaining -= br;
    return br;
}


static ui16 imageBuf [ST7789_WIDTH * ST7789_HEIGHT];
ui32 imageBufPointer = 0;


INT tjd_output (JDEC *jd, void *bitmap, JRECT *rect)
{
    uint16_t* buf = (uint16_t*)bitmap;

    for (ui16 i = 0; i < (rect->right - rect->left + 1) * (rect->bottom - rect->top + 1); i++)
    {
    	buf[i] = __REV16(buf[i]);
    }

    if ((rect->left >= ST7789_WIDTH) || (rect->right >= ST7789_WIDTH))
		return -1;  // continue

    if ((rect->top) >= ST7789_HEIGHT || (rect->bottom >= ST7789_HEIGHT))
    	return 0;  // break

    for (ui16 i = 0; i <= (rect->bottom - rect->top); i++)
    	memcpy(&imageBuf[((i + rect->top) * ST7789_HEIGHT) + rect->left], &buf[i * ((rect->right - rect->left) + 1)], ((rect->right - rect->left) + 1) * 2);

    return -1;
}



#pragma pack(push, 1)

typedef struct
{
    uint16_t bfType;      // 'BM'
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BMPHeader;

typedef struct
{
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BMPInfoHeader;

#pragma pack(pop)


FRESULT write_bmp_to_file(const char *filename, uint16_t *framebuffer, int width, int height)
{
    FIL file;
    FRESULT res;
    UINT written;

    BMPHeader header;
    BMPInfoHeader info;

    uint32_t row_size = width * 2;  // RGB565: 2 bytes на пиксель
    uint32_t image_size = row_size * height;
    uint32_t file_size = sizeof(BMPHeader) + sizeof(BMPInfoHeader) + image_size;

    // --- Заполнение заголовков ---
    header.bfType = 0x4D42; // 'BM'
    header.bfSize = file_size;
    header.bfReserved1 = 0;
    header.bfReserved2 = 0;
    header.bfOffBits = sizeof(BMPHeader) + sizeof(BMPInfoHeader);

    info.biSize = sizeof(BMPInfoHeader);
    info.biWidth = width;
    info.biHeight = -height; // Отрицательное значение: данные идут от верхней строки
    info.biPlanes = 1;
    info.biBitCount = 16;  // RGB565
    info.biCompression = 3; // BI_BITFIELDS
    info.biSizeImage = image_size;
    info.biXPelsPerMeter = 0;
    info.biYPelsPerMeter = 0;
    info.biClrUsed = 0;
    info.biClrImportant = 0;

    // Цветовая маска для RGB565
    const uint32_t masks[3] = { 0xF800, 0x07E0, 0x001F };

    // --- Открытие файла ---
    res = f_open(&file, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) return res;

    // --- Запись заголовков ---
    f_write(&file, &header, sizeof(header), &written);
    f_write(&file, &info, sizeof(info), &written);
    f_write(&file, masks, sizeof(masks), &written);  // маски для RGB565

    // --- Запись пикселей ---
    for (int y = 0; y < height; y++)
    {
        uint16_t *row = &framebuffer[width * y];

        for (ui16 i = 0; i < width; i++)
		{
        	row[i] = __REV16(row[i]);
		}

        f_write(&file, row, row_size, &written);
    }

    f_close(&file);
    return FR_OK;
}


FRESULT make_file_hidden(const char *filename)
{
    return f_chmod(filename, AM_HID, AM_HID);
}


NORETURN__ uPlayerStatus_t qc_uPlayer::Start (void)
{
	Init();
	// todo parse .playlist file

	FIL JPEG_File;

	f_open(&JPEG_File, "1.jpg", FA_READ);

	jpeg_source_t jpegSrc =
	{
		.file = &JPEG_File,
		.remaining = 10000000
	};

	JDEC jd; uint8_t work [1024 * 64];

	ui32 time = qmGetTick();

	if (jd_prepare(&jd, tjd_input, work, sizeof(work), &jpegSrc) == JDR_OK)
		jd_decomp(&jd, tjd_output, 1);  // 1 - 1/2, 2 - 1/4, 3 - 1/8


	ui32 difTime = qmGetTick() - time;

	uPlayer.ST7789_DrawImage(0, 0, 240, 240, imageBuf);

	char difTimeStr [20];

	sprintf_(difTimeStr, "%d", difTime);

	write_bmp_to_file("bufferImage.bmp", imageBuf, 240, 240);

	make_file_hidden("bufferImage.bmp");

//	ST7789_WriteString(2, 2, difTimeStr, &Font20, GREEN, BLACK);

	while (1);


//	InitFolder("/flac/Scaled and Icy");
//	InitFolder("/flac/Trench");
//	InitFolder("/flac/Blurryface");
//	InitFolder("/flac/Home");
	InitFolder("/flac/Bounce Into The Music");


//	InitFolder("/wav");
//	InitFolder("/mp3");
//	InitFolder("/cmp");

	SetEvent(uPL_EVENT_TRACK_PAUSE_PLAY);

	while (1)
	{
		if (state == uPL_PLAY)
			Play();

		ButtonsHandle();
		EventHandler();
	}
}
