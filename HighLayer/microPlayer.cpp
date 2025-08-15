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


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														  Global Objects
 */
qc_uPlayer uPlayer;


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														Static Functions
 */

static size_t DrLibRead (void *pUserData, void *pBufferOut, size_t bytesToRead)
{
    FIL *file = (FIL*)pUserData;
    UINT bytesRead;
    FRESULT res = f_read(file, pBufferOut, bytesToRead, &bytesRead);

    return (res == FR_OK) ? bytesRead : 0;
}


static unsigned int DrLibTell (void *pUserData, signed long long *pCursor)
{
	FIL *file = (FIL*)pUserData;

	*pCursor = f_tell(file);

	return 1;
}


static unsigned int DrLibSeek (void *pUserData, int offset, int origin)
{
    FIL *file = (FIL*)pUserData;

    return (f_lseek_ext(file, offset, origin) == FR_OK) ? 1 : 0;
}

static drmp3_bool32 DrMP3_Seek (void *pUserData, int offset, drmp3_seek_origin origin)
{
	return DrLibSeek(pUserData, offset, origin);
}

static drflac_bool32 DrFLAC_Seek (void *pUserData, int offset, drflac_seek_origin origin)
{
	return DrLibSeek(pUserData, offset, origin);
}

static drwav_bool32 DrWAV_Seek (void *pUserData, int offset, drwav_seek_origin origin)
{
	return DrLibSeek(pUserData, offset, origin);
}


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   Class Methods
 */
uPlayerStatus_t qc_uPlayer::OpenMP3 (void)
{
	MP3MetaInfo(trackPath);

	PrintTrackInfo();
	PrintTrackTime();

	track = qmCreate(drmp3);

	if (drmp3_init((drmp3*)track, DrLibRead, DrMP3_Seek, DrLibTell, NULL, &file, NULL) != DRMP3_TRUE)
	{
		qmDestroy(track);

		error = uPL_TRACK_OPEN_ERROR;

		return uPL_ERROR;
	}

	PCM5142_SetSampleRate(((drmp3*)track)->sampleRate);
	PCM5142_SetBitRate(16);

	PCM_Buffer = qmCreateArr(i16, qcfgPCM_BUFFER_SIZE * sizeof(i16));

	bufferOffset = BUFFER_OFFSET_FULL;

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::OpenFLAC (void)
{
	FlacMetaInfo(trackPath);
	PrintTrackInfo();
	PrintTrackTime();

	track = drflac_open(DrLibRead, DrFLAC_Seek, DrLibTell, &file, NULL);

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
	WAVMetaInfo(trackPath);
	PrintTrackInfo();
	PrintTrackTime();

	track = qmCreate(drwav);

	drwav_init((drwav*)track, DrLibRead, DrWAV_Seek, DrLibTell, &file, NULL);

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
	if (bufferOffset != BUFFER_OFFSET_NONE)
	{
		if (bufferOffset == BUFFER_OFFSET_FULL)
			framesRead = drmp3_read_pcm_frames_s16((drmp3*)track, qcfgPCM_BUFFER_SIZE / 4, (drmp3_int16*)PCM_Buffer);

		else
			framesRead = drmp3_read_pcm_frames_s16((drmp3*)track, qcfgPCM_BUFFER_SIZE / 4,
					(drmp3_int16*)(((drmp3_int16*)PCM_Buffer) + (qcfgPCM_BUFFER_SIZE / 2)));

		bufferOffset = BUFFER_OFFSET_NONE;
	}

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::ReadFLAC_16 (void)
{
	if (bufferOffset != BUFFER_OFFSET_NONE)
	{
		if (bufferOffset == BUFFER_OFFSET_FULL)
			framesRead = drflac_read_pcm_frames_s16((drflac*)track, qcfgPCM_BUFFER_SIZE / 4, (drflac_int16*)PCM_Buffer);

		else
			framesRead = drflac_read_pcm_frames_s16((drflac*)track, qcfgPCM_BUFFER_SIZE / 4,
					(drflac_int16*)(((drflac_int16*)PCM_Buffer) + (qcfgPCM_BUFFER_SIZE / 2)));

		bufferOffset = BUFFER_OFFSET_NONE;
	}

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::ReadFLAC_32 (void)
{
	if (bufferOffset != BUFFER_OFFSET_NONE)
	{
		if (bufferOffset == BUFFER_OFFSET_FULL)
			framesRead = drflac_read_pcm_frames_s32((drflac*)track, qcfgPCM_BUFFER_SIZE / 4, (drflac_int32*)PCM_Buffer);

		else
			framesRead = drflac_read_pcm_frames_s32((drflac*)track, qcfgPCM_BUFFER_SIZE / 4,
					(drflac_int32*)(((drflac_int32*)PCM_Buffer) + (qcfgPCM_BUFFER_SIZE / 2)));

		bufferOffset = BUFFER_OFFSET_NONE;
	}

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::ReadWAV_16 (void)
{
	if (bufferOffset != BUFFER_OFFSET_NONE)
	{
		if (bufferOffset == BUFFER_OFFSET_FULL)
			framesRead = drwav_read_pcm_frames_s16((drwav*)track, qcfgPCM_BUFFER_SIZE / 4, (drwav_int16*)PCM_Buffer);

		else
			framesRead = drwav_read_pcm_frames_s16((drwav*)track, qcfgPCM_BUFFER_SIZE / 4,
					(drwav_int16*)(((drwav_int16*)PCM_Buffer) + (qcfgPCM_BUFFER_SIZE / 2)));

		bufferOffset = BUFFER_OFFSET_NONE;
	}

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::ReadWAV_32 (void)
{
	if (bufferOffset != BUFFER_OFFSET_NONE)
	{
		if (bufferOffset == BUFFER_OFFSET_FULL){
			framesRead = drwav_read_pcm_frames_s32((drwav*)track, qcfgPCM_BUFFER_SIZE / 4, (drwav_int32*)PCM_Buffer);
		}
		else{
			framesRead = drwav_read_pcm_frames_s32((drwav*)track, qcfgPCM_BUFFER_SIZE / 4,
			(drwav_int32*)(((drwav_int32*)PCM_Buffer) + (qcfgPCM_BUFFER_SIZE / 2)));
		}

		bufferOffset = BUFFER_OFFSET_NONE;
	}

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::CloseMP3 (void)
{
	drmp3_uninit((drmp3*)track);
	f_close(&file);

	qmDestroy(PCM_Buffer);
	qmDestroy(track);

	mp3_meta.flag = false;

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::CloseFLAC (void)
{
	drflac_close((drflac*)track);
	f_close(&file);

	qmDestroy(PCM_Buffer);
	qmDestroy(track);

	flac_meta.flag = false;

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::CloseWAV (void)
{
	drwav_uninit((drwav*)track);
	f_close(&file);

	qmDestroy(PCM_Buffer);
	qmDestroy(track);

	wav_meta.flag = false;

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
	ADC_Init();

	/* All buttons init */
	ButtonsInit();

	/* RealTimeClock init*/
	//RTC_Init();

	/* Menu init*/
	InitMenu(0);
	menu.index = 0;
	menu.where = 0;


	Buttons[BUT_VOL_DOWN].OnPress = [](void *param)
	{
		static_cast<qc_uPlayer*>(param)->OnVolumeDownPress(nullptr);
	};

			Buttons[BUT_VOL_DOWN].OnHold = [](void *param)
			{
				static_cast<qc_uPlayer*>(param)->OnVolumeDownHold(nullptr);
			};

	Buttons[BUT_VOL_UP].OnPress = [](void *param)
	{
		static_cast<qc_uPlayer*>(param)->OnVolumeUpPress(nullptr);
	};

			Buttons[BUT_VOL_UP].OnHold = [](void *param)
			{
				static_cast<qc_uPlayer*>(param)->OnVolumeUpHold(nullptr);
			};

	Buttons[BUT_TRACK_PLAY].OnPress = [](void *param)
	{
		static_cast<qc_uPlayer*>(param)->OnTrackPlayPress(nullptr);
	};

			Buttons[BUT_TRACK_PLAY].OnHold = [](void *param)
			{
				static_cast<qc_uPlayer*>(param)->OnTrackPlayHold(nullptr);
			};

				Buttons[BUT_TRACK_PLAY].OnLastHold = [](void *param)
				{
					static_cast<qc_uPlayer*>(param)->OnTrackPlayLastHold(nullptr);
				};

	Buttons[BUT_TRACK_BACK].OnPress = [](void *param)
	{
		static_cast<qc_uPlayer*>(param)->OnTrackBackPress(nullptr);
	};

			Buttons[BUT_TRACK_BACK].OnHold = [](void *param)
			{
				static_cast<qc_uPlayer*>(param)->OnTrackBackHold(nullptr);
			};

	Buttons[BUT_TRACK_NEXT].OnPress = [](void *param)
	{
		static_cast<qc_uPlayer*>(param)->OnTrackNextPress(nullptr);
	};

			Buttons[BUT_TRACK_NEXT].OnHold = [](void *param)
			{
				static_cast<qc_uPlayer*>(param)->OnTrackNextHold(nullptr);
			};

	for (ui8 i = 0; i < BUT_NUMBER; i++)
		Buttons[i].pThis = this;

	/* Mount file system : "0:/\0" */
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

	error = uPL_ERROR_NONE;
	state = uPL_STOP;

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::InitFolder(const char *folder)
{
	ui16 fileNameLength = 0;
	ui32 totalFileNameLength = 0;

	totalTrackNumber = 0;

	FRESULT res = f_chdir(folder);
	res = f_findfirst(&fatFS.dir, &fatFS.fno, folder, "*");

	/* First pass: count valid audio files */
	while (res == FR_OK && fatFS.fno.fname[0])
	{
		if (!(fatFS.fno.fattrib & AM_DIR))  // Skip folders
		{
			const char *name = fatFS.fno.fname;
			const char *ext = strrchr(name, '.');

			if (ext &&
				((!strcasecmp(ext, ".mp3") ||
				!strcasecmp(ext, ".flac") || !strcasecmp(ext, ".ogg")) ||
				!strcasecmp(ext, ".wav") || !strcasecmp(ext, ".wv")))
			{
				totalTrackNumber++;
				totalFileNameLength += strlen(name) + 1;
			}
		}

		res = f_findnext(&fatFS.dir, &fatFS.fno);
	}

	fatFS.playlistTrackNames = qmCreateArr(char, totalFileNameLength);
	fatFS.playlistTrackNamePointers = qmCreateArr(ui16, totalTrackNumber * 2 + 2);

	totalFileNameLength = 0;
	res = f_findfirst(&fatFS.dir, &fatFS.fno, folder, "*");

	/* Second pass: store valid audio file names */
	ui32 i = 0;

	while (res == FR_OK && fatFS.fno.fname[0])
	{
		if (!(fatFS.fno.fattrib & AM_DIR))
		{
			const char *name = fatFS.fno.fname;
			const char *ext = strrchr(name, '.');

			if (ext &&
				((!strcasecmp(ext, ".mp3") ||
				!strcasecmp(ext, ".flac") || !strcasecmp(ext, ".ogg")) ||
				!strcasecmp(ext, ".wav") || !strcasecmp(ext, ".wv")))
			{
				fileNameLength = strlen(name) + 1;

				fatFS.playlistTrackNamePointers[i] = totalFileNameLength;

				memcpy(&fatFS.playlistTrackNames[totalFileNameLength], name, fileNameLength);
				totalFileNameLength += fileNameLength;

				i++;
			}
		}

		res = f_findnext(&fatFS.dir, &fatFS.fno);
	}

	fatFS.playlistTrackNamePointers[i] = totalFileNameLength;

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::InitCodec (void)
{
	/* Load current track from playlist */
	trackPath = &fatFS.playlistTrackNames[fatFS.playlistTrackNamePointers[trackNumber]];

	if (f_open(&file, trackPath, FA_READ) != FR_OK)
	{
		SetEvent(uPL_EVENT_TRACK_NEXT);
		status = uPL_ERROR;

		error = uPL_FILE_OPEN_ERROR;
		currentCodec = uPLFC_NONE;

		return uPL_ERROR;  // Not supported format
	}

	/* Check current audio file */
	const char *ext = strrchr(trackPath, '.');
	uPlayerFileCodec_t fileCodec = uPLFC_NONE;

	if (ext)
	{
		if (!strcasecmp(ext, ".mp3"))
			fileCodec = uPLFC_MP3;

		else if (!strcasecmp(ext, ".flac") || !strcasecmp(ext, ".ogg"))
			fileCodec = uPLFC_FLAC;

		else if (!strcasecmp(ext, ".wav") || !strcasecmp(ext, ".wv"))
			fileCodec = uPLFC_WAV;
	}

	else
	{
		f_close(&file);

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
	if (qmCall(OpenTrack) != uPL_OK)
	{
		f_close(&file);

		status = uPL_ERROR;

		return uPL_ERROR;
	}

	qmCall(ReadFrames);
	bufferOffset = BUFFER_OFFSET_HALF;

	/* Unmute */
	PCM5142_Unmute();

	/* Start DMA */
	HAL_I2S_Transmit_DMA(&DAC_I2S_HANDLE, (ui16*)PCM_Buffer, qcfgPCM_BUFFER_SIZE);

	state = uPL_PLAY;


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


void qc_uPlayer::OnVolumeDownPress (void *param)
{
	SetEvent(uPL_EVENT_VOLUME_DOWN);
}


void qc_uPlayer::OnVolumeDownHold (void *param)
{

}


void qc_uPlayer::OnVolumeUpPress (void *param)
{
	SetEvent(uPL_EVENT_VOLUME_UP);
}


void qc_uPlayer::OnVolumeUpHold (void *param)
{

}


void qc_uPlayer::OnTrackPlayPress (void *param)
{
	SetEvent(uPL_EVENT_TRACK_PAUSE_PLAY);
}


void qc_uPlayer::OnTrackPlayHold (void *param)
{
	SetEvent(uPL_EVENT_TRACK_PAUSE_PLAY_HOLD);
}


void qc_uPlayer::OnTrackPlayLastHold (void *param)
{
	;
}


void qc_uPlayer::OnTrackBackPress (void *param)
{
	SetEvent(uPL_EVENT_TRACK_BACK);
}


void qc_uPlayer::OnTrackBackHold (void *param)
{

}


void qc_uPlayer::OnTrackNextPress (void *param)
{
	SetEvent(uPL_EVENT_TRACK_NEXT);
}


void qc_uPlayer::OnTrackNextHold (void *param)
{

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
			if (StatusDisplay == uPL_STATUS_PLAYER)
			{
				Back();
			}
			if (StatusDisplay == uPL_STATUS_MENU)
			{
				if (menu.where == 0){
					if (menu.index == 0)
						menu.index = MAX_COMPONENTS - 1;
					else
						menu.index--;
					DrawMenu(menu.index);
				}
				else if (menu.where == 1){
					if (menu.subindex == 0)
						menu.subindex = folderCount - 1;
					else
						menu.subindex--;
					DrawFolderMenu(menu.subindex);
				}
				else if (menu.where == 2){
					if (menu.subindex == 0)
						menu.subindex = MAX_LANGUAGES - 1;
					else
						menu.subindex--;
					Language(menu.subindex);
				}
			}
			ResetEvent(uPL_EVENT_TRACK_BACK);
			break;

		case uPL_EVENT_TRACK_NEXT :
			if (StatusDisplay == uPL_STATUS_PLAYER)
			{
				Next();
			}
			if (StatusDisplay == uPL_STATUS_MENU){
				if (menu.where == 0){
					if (menu.index == (MAX_COMPONENTS - 1)) menu.index = 0;
					else menu.index++;
					DrawMenu(menu.index);
				}
				else if (menu.where == 2){
					if (menu.subindex == (MAX_LANGUAGES - 1)) menu.subindex = 0;
					else menu.subindex++;
					Language(menu.subindex);
				}
				else if (menu.where == 1){
					if (menu.subindex == (folderCount - 1)) menu.subindex = 0;
					else menu.subindex++;
					DrawFolderMenu(menu.subindex);
				}
			}
			ResetEvent(uPL_EVENT_TRACK_NEXT);
			break;

		case uPL_EVENT_TRACK_PAUSE_PLAY_HOLD :
			if (StatusDisplay == uPL_STATUS_PLAYER)
			{
				Buttons[BUT_TRACK_PLAY].status = BS_RELEASED;
				DeinitCodec();
				StatusDisplay = uPL_STATUS_MENU;

				DrawMenu(menu.index);
			}
			else if (StatusDisplay == uPL_STATUS_MENU)
			{
				Buttons[BUT_TRACK_PLAY].status = BS_RELEASED;
				StatusDisplay = uPL_STATUS_PLAYER;
				menu.where = 0;
				menu.index = 0;
				menu.subindex = 0;

				InitCodec();

				ST7789_FillColor(BLACK);
				PrintTrackInfo();
				PrintTrackTime();
			}
			ResetEvent(uPL_EVENT_TRACK_PAUSE_PLAY_HOLD);
			break;
		case uPL_EVENT_TRACK_PAUSE_PLAY :
			if (StatusDisplay == uPL_STATUS_PLAYER)
			{
				if (state == uPL_STOP)
					InitCodec();

				else if (state == uPL_PAUSE)
				{
					ST7789_DrawFilledRectangle(0, 0, 240, 19, BLACK);
					ST7789_DrawFilledRectangle(193, 220, 47, 20, BLACK);
					PrintTrackTime();

					PCM5142_Unmute();
					HAL_I2S_DMAResume(&DAC_I2S_HANDLE);

					state = uPL_PLAY;
				}

				else if (state == uPL_PLAY)
				{
					Pause();
					DrawAttributes();
					state = uPL_PAUSE;
				}
			}

			if (StatusDisplay == uPL_STATUS_MENU)
			{
				if 		(menu.where == 0) 	Transition(menu.index);
				else if (menu.where == 1)	{
					DeinitCodec();
				}
				else if (menu.where == 2) 	SelectLang(menu.subindex);
				else if (menu.where == 4) {
											menu.where = 0;
											DrawMenu(menu.index);
				}
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

	ShowVolumeBar(volume);

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

	ShowVolumeBar(volume);

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

	//HAL_I2S_DMAPause(&uPlayer.DAC_I2S_HANDLE);

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


NORETURN__ uPlayerStatus_t qc_uPlayer::Start (void)
{
	Init();

	ScanFolders();//"flac"
	selectedFolderIndex = 0;

	FRESULT res;

	char original_path [64] = { 0 };

	res = f_getcwd(original_path, sizeof(original_path));

	res = f_mkdir("0:/cache");
	res = f_chmod("0:/cache", AM_HID, AM_HID);

	res = f_mkdir(qcfgCACHE_IMG_PATH);

//	InitFolder("/flac/Bounce Into The Music");
	InitFolder("/mymusic/mp3nik");
//	InitFolder("/mymusic/wav");
//	InitFolder("/mymusic/flac");

	ui32 timeBattery = 0, volumeBarTime = 0;
	StatusDisplay = uPL_STATUS_PLAYER;
	SetEvent(uPL_EVENT_TRACK_PAUSE_PLAY);

	while (1)
	{
		if (state == uPL_PLAY)
			Play();

		if (qmGetTick() - timeBattery > 1000) {
			batteryLevel = ReadBatteryLevel();
			timeBattery = qmGetTick();
		}

		if (qmGetTick() - volumeBarTick > qcfgVOLUME_BAR_TIMER && flagVolBar)
		{
			HideVolumeBar();
		}

		ButtonsHandle();
		EventHandler();
	}
}
