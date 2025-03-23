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

#define		DR_FLAC_IMPLEMENTATION
#include	"dr_flac.h"

//#define DR_MP3_IMPLEMENTATION
//#include "dr_mp3.h"

#define		DR_WAV_IMPLEMENTATION
#include	"dr_wav.h"


FRESULT res;
FILINFO fno;
DIR dir;

char fullPath [262];
char fullPathPrev [262];

extern ui8 dirFlag;


#if !defined DR_MP3_IMPLEMENTATION
	FIL MP3_File;
	helix_mp3_t mp3;
#else
	drmp3 mp3;
#endif


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


uPlayerStatus_t qc_uPlayer::Play (void)
{
	res = f_findfirst(&dir, &fno, "/mp3", "*");

	while (res == FR_OK && fno.fname[0])
	{
		if (fno.fattrib & AM_DIR)
			;

		else
		{
			if (dirFlag)
			{
				memcpy(fullPathPrev, fullPath, sizeof(fullPath));
				memset(fullPath, 0, sizeof(fullPath));

				sprintf(fullPath, "/mp3/%s", fno.fname);
			}

			else
			{
				memcpy(fullPath, fullPathPrev, sizeof(fullPath));
				dirFlag = 1;
			}

			// todo Define file type and open the respective decoder
			// Assign general decoder functions

			#if !defined DR_MP3_IMPLEMENTATION
				helix_mp3_init_file(&mp3, fullPath);
			#else
				drmp3_init_file(&mp3, fullPath, NULL);
			#endif


			#if !defined DR_MP3_IMPLEMENTATION
				DAC_SetSampleRate(helix_mp3_get_sample_rate(&mp3));
			#else
				DAC_SetSampleRate(mp3.sampleRate)
			#endif


			#if !defined DR_MP3_IMPLEMENTATION
				volatile size_t bytesRead = helix_mp3_read_pcm_frames_s16(&mp3, PCM_Buffer, PCM_BUFFER_SIZE / 2);
			#else
				size_t bytesRead = drmp3_read_pcm_frames_s16(&mp3, PCM_BUFFER_SIZE / 2, PCM_Buffer);
			#endif

			/* Unmute */
			DAC_SetMute(AUDIO_MUTE_OFF);

			HAL_I2S_Transmit_DMA(&DAC_I2S_HANDLE, (ui16*)PCM_Buffer, PCM_BUFFER_SIZE);

			/* Decode the whole file */
			while (bytesRead)
			{
				if (dirFlag == 2)
				{
					dirFlag = 1;
					break;
				}

				else if (dirFlag == 0)
					break;

				if (bufferOffset == BUFFER_OFFSET_FULL)
				{
					#if !defined DR_MP3_IMPLEMENTATION
						bytesRead = helix_mp3_read_pcm_frames_s16(&mp3, PCM_Buffer, PCM_BUFFER_SIZE / 4);
					#else
						bytesRead = drmp3_read_pcm_frames_s16(&mp3, PCM_BUFFER_SIZE / 4, PCM_Buffer);
					#endif

					bufferOffset = BUFFER_OFFSET_NONE;
				}

				if (bufferOffset == BUFFER_OFFSET_HALF)
				{
					#if !defined DR_MP3_IMPLEMENTATION
						bytesRead = helix_mp3_read_pcm_frames_s16(&mp3, &PCM_Buffer[PCM_BUFFER_SIZE / 2], PCM_BUFFER_SIZE / 4);
					#else
						bytesRead = drmp3_read_pcm_frames_s16(&mp3, PCM_BUFFER_SIZE / 4, &PCM_Buffer[PCM_BUFFER_SIZE / 2]);
					#endif

					bufferOffset = BUFFER_OFFSET_NONE;
				}
			}

			DAC_SetMute(AUDIO_MUTE_ON);

			#if !defined DR_MP3_IMPLEMENTATION
				helix_mp3_deinit(&mp3);
			#else
				drmp3_uninit(&mp3);
			#endif
		}
		// Переход к следующему элементу

		if (dirFlag)
			res = f_findnext(&dir, &fno);
	}

	return uPL_OK;
}
