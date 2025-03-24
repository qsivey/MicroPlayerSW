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

#include	"helix_mp3.h"

#include	"dr_mp3.h"
#include	"dr_flac.h"
#include	"dr_wav.h"

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
	#if !defined DR_MP3_IMPLEMENTATION

		track = qmCreate(helix_mp3_t);

		helix_mp3_init_file((helix_mp3_t*)track, trackPath);

		DAC_SetSampleRate(helix_mp3_get_sample_rate((helix_mp3_t*)track));

	#else
		drmp3_init_file(track, trackPath, NULL);

		DAC_SetSampleRate(track.sampleRate)
	#endif

	PCM_Buffer = qmCreateArr(i16, qcfgPCM_BUFFER_SIZE * 2);

	bufferOffset = BUFFER_OFFSET_FULL;

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::OpenFLAC (void)
{
	track = drflac_open_file(trackPath, NULL);

	DAC_SetSampleRate(((drflac*)track)->sampleRate);

	if (((drflac*)track)->bitsPerSample <= 16)
	{
		ReadFrames = &qc_uPlayer::ReadFLAC_16;
		PCM_Buffer = qmCreateArr(drflac_int16, qcfgPCM_BUFFER_SIZE * 2);
	}

	else
	{
		ReadFrames = &qc_uPlayer::ReadFLAC_32;
		PCM_Buffer = qmCreateArr(drflac_int32, qcfgPCM_BUFFER_SIZE * 4);
	}

	bufferOffset = BUFFER_OFFSET_FULL;

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::OpenWAV (void)
{
	track = qmCreate(drwav);

	drwav_init_file((drwav*)track, trackPath, NULL);

	DAC_SetSampleRate(((drwav*)track)->sampleRate);

	if (((drwav*)track)->bitsPerSample <= 16)
	{
		ReadFrames = &qc_uPlayer::ReadWAV_16;
		PCM_Buffer = qmCreateArr(drwav_int16, qcfgPCM_BUFFER_SIZE * 2);
	}

	else
	{
		ReadFrames = &qc_uPlayer::ReadWAV_32;
		PCM_Buffer = qmCreateArr(drwav_int32, qcfgPCM_BUFFER_SIZE * 4);
	}

	bufferOffset = BUFFER_OFFSET_FULL;

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::ReadMP3 (void)
{
	#if !defined DR_MP3_IMPLEMENTATION

		if (bufferOffset == BUFFER_OFFSET_FULL)
			framesRead = helix_mp3_read_pcm_frames_s16((helix_mp3_t*)track, (i16*)PCM_Buffer, qcfgPCM_BUFFER_SIZE / 4);

		else if (bufferOffset == BUFFER_OFFSET_HALF)
			framesRead = helix_mp3_read_pcm_frames_s16((helix_mp3_t*)track,
					(i16*)(((i16*)PCM_Buffer) + (qcfgPCM_BUFFER_SIZE / 2)), qcfgPCM_BUFFER_SIZE / 4);

	#else
		size_t bytesRead = drmp3_read_pcm_frames_s16(track, qcfgPCM_BUFFER_SIZE / 2, PCM_Buffer);
	#endif

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
	// todo check
	if (bufferOffset == BUFFER_OFFSET_FULL)
	{
		framesRead = drflac_read_pcm_frames_s32((drflac*)track, qcfgPCM_BUFFER_SIZE / 4, (drflac_int32*)PCM_Buffer);

		bufferOffset = BUFFER_OFFSET_HALF;
	}

	else
	{
		framesRead = drflac_read_pcm_frames_s32((drflac*)track, qcfgPCM_BUFFER_SIZE / 4,
				(drflac_int32*)(((drflac_int32*)PCM_Buffer) + (qcfgPCM_BUFFER_SIZE / 2)));

		bufferOffset = BUFFER_OFFSET_FULL;
	}

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
	// todo check
	if (bufferOffset == BUFFER_OFFSET_FULL)
	{
		framesRead = drwav_read_pcm_frames_s32((drwav*)track, qcfgPCM_BUFFER_SIZE / 4, (drwav_int32*)PCM_Buffer);

		bufferOffset = BUFFER_OFFSET_HALF;
	}

	else
	{
		framesRead = drwav_read_pcm_frames_s32((drwav*)track, qcfgPCM_BUFFER_SIZE / 4,
				(drwav_int32*)(((drwav_int32*)PCM_Buffer) + (qcfgPCM_BUFFER_SIZE / 2)));

		bufferOffset = BUFFER_OFFSET_FULL;
	}

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::CloseMP3 (void)
{
	#if !defined DR_MP3_IMPLEMENTATION
		helix_mp3_deinit((helix_mp3_t*)track);
	#else
		drmp3_uninit(&mp3);
	#endif

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

	/* All buttons init */
	ButtonsInit();

	/* Mount file system */
	extern Diskio_drvTypeDef SD_Driver [];

	FATFS_LinkDriver(SD_Driver, fatFS.SD_Path);

	if (f_mount(&fatFS.fs, (TCHAR const*)fatFS.SD_Path, 0) != FR_OK)
		return uPL_ERROR;

	/* CS43L22 init */
	DAC_Init();

	/* Service variable init */
	currentCodec = uPLFC_NONE;
	trackNumber = 0;

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
	fatFS.playlistTrackNamePointers = qmCreateArr(ui16, totalTrackNumber * 2);

	totalFileNameLength = 0;  // For now this is the cursor

	res = f_findfirst(&fatFS.dir, &fatFS.fno, folder, "*");

	/* Write file names and assign the pointers */
	for (ui32 i = 0; (res == FR_OK) && (fatFS.fno.fname[0]); i++)
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

	else if (((qsbCurTrNameEndOfs(4) == 'f') && (qsbCurTrNameEndOfs(3) == 'l') && (qsbCurTrNameEndOfs(2) == 'a') && (qsbCurTrNameEndOfs(1) == 'c')) ||
		((qsbCurTrNameEndOfs(4) == 'F') && (qsbCurTrNameEndOfs(3) == 'L') && (qsbCurTrNameEndOfs(2) == 'A') && (qsbCurTrNameEndOfs(1) == 'C')))
		fileCodec = uPLFC_FLAC;

	else if (((qsbCurTrNameEndOfs(3) == 'w') && (qsbCurTrNameEndOfs(2) == 'a') && (qsbCurTrNameEndOfs(1) == 'v')) ||
		((qsbCurTrNameEndOfs(3) == 'W') && (qsbCurTrNameEndOfs(2) == 'A') && (qsbCurTrNameEndOfs(1) == 'V')))
		fileCodec = uPLFC_WAV;

	else
		return uPL_ERROR;  // Not supported format

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
	DAC_SetMute(AUDIO_MUTE_OFF);

	/* Start DMA */
	HAL_I2S_Transmit_DMA(&DAC_I2S_HANDLE, (ui16*)PCM_Buffer, qcfgPCM_BUFFER_SIZE);

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::DeinitCodec (void)
{
	/* Mute */
	DAC_SetMute(AUDIO_MUTE_ON);
	qmDelayMs(20);

	/* Stoop DMA */
	HAL_I2S_DMAStop(&DAC_I2S_HANDLE);

	/* Close current track */
	qmCall(CloseTrack);

	return uPL_OK;
}


uPlayerStatus_t	qc_uPlayer::EventHandler (void)
{
	if (event == uPL_EVENT_NONE)
		return uPL_OK;

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
			{
				InitCodec();

				state = uPL_PLAY;
			}

			else if (state == uPL_PAUSE)
			{
				DAC_SetMute(AUDIO_MUTE_OFF);
				DAC_Write(CS43L22_REG_POWER_CTL1, 0x9E);

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
	volume += qcfgVOLUME_PITCH;

	volume = constrain_(volume, qcfgMIN_VOLUME, qcfgMAX_VOLUME);

	DAC_SetVolume(volume);

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::VolumeDown (void)
{
	if (qcfgMIN_VOLUME + qcfgVOLUME_PITCH > volume)
		volume = qcfgMIN_VOLUME;

	else
		volume -= qcfgVOLUME_PITCH;

	DAC_SetVolume(volume);

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
	DAC_SetMute(AUDIO_MUTE_ON);
	DAC_Write(CS43L22_REG_POWER_CTL1, 0x01);
	qmDelayMs(20);

	HAL_I2S_DMAPause(&uPlayer.DAC_I2S_HANDLE);

	return uPL_OK;
}


uPlayerStatus_t qc_uPlayer::Play (void)
{
	/* Decode next frame */
	qmCall(ReadFrames);

	if (!framesRead)
		Next();

	return uPL_OK;
}


NORETURN__ uPlayerStatus_t qc_uPlayer::Task (void)
{
	Init();

	// todo parse .playlist file

	InitFolder("/mp3");

	while (1)
	{
		if (state == uPL_PLAY)
			Play();

		ButtonsHandle();

		EventHandler();
	}
}
