/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#ifndef		OUTPUTS_CS43L22_H_
#define		OUTPUTS_CS43L22_H_

#include	"projectConfig.h"
#include	"periph.h"


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *													   CS43L22 Constants
 */
#define		CS43L22_REG_POWER_CTRL1			0x02
#define		CS43L22_REG_POWER_CTRL2			0x04
#define		CS43L22_REG_CLOCK_CTRL			0x05
#define		CS43L22_REG_INTERFACE_CTRL1		0x06
#define		CS43L22_REG_MASTER_A_VOL		0x20
#define		CS43L22_REG_MASTER_B_VOL		0x21
#define		CS43L22_REG_PLAYBACK_CTRL		0x0D
#define		CS43L22_REG_MISC_CTRL			0x0E

/* Codec output DEVICE */
#define		CS43L22_OUTPUT_DEVICE_SPEAKER	1
#define		CS43L22_OUTPUT_DEVICE_HEADPHONE	2
#define		CS43L22_OUTPUT_DEVICE_BOTH		3
#define		CS43L22_OUTPUT_DEVICE_AUTO		4

/* Volume Levels values */
#define		CS43L22_DEFAULT_VOLMIN			0x00
#define		CS43L22_DEFAULT_VOLMAX			0xFF
#define		CS43L22_DEFAULT_VOLSTEP			0x04

#define		CS43L22_AUDIO_PAUSE				0
#define		CS43L22_AUDIO_RESUME			1

/* Codec POWER DOWN modes */
#define		CS43L22_CODEC_PDWN_HW			1
#define		CS43L22_CODEC_PDWN_SW			2

/* MUTE commands */
#define		CS43L22_AUDIO_MUTE_ON			1
#define		CS43L22_AUDIO_MUTE_OFF			0

/* AUDIO FREQUENCY */
#define		CS43L22_AUDIO_FREQUENCY_192K	((uint32_t)192000)
#define		CS43L22_AUDIO_FREQUENCY_96K		((uint32_t)96000)
#define		CS43L22_AUDIO_FREQUENCY_48K		((uint32_t)48000)
#define		CS43L22_AUDIO_FREQUENCY_44K		((uint32_t)44100)
#define		CS43L22_AUDIO_FREQUENCY_32K		((uint32_t)32000)
#define		CS43L22_AUDIO_FREQUENCY_22K		((uint32_t)22050)
#define		CS43L22_AUDIO_FREQUENCY_16K		((uint32_t)16000)
#define		CS43L22_AUDIO_FREQUENCY_11K		((uint32_t)11025)
#define		CS43L22_AUDIO_FREQUENCY_8K		((uint32_t)8000)

/* CS43L22 Registers */
#define		CS43L22_REG_ID					0x01
#define		CS43L22_REG_POWER_CTL1			0x02
#define		CS43L22_REG_POWER_CTL2			0x04
#define		CS43L22_REG_CLOCKING_CTL		0x05
#define		CS43L22_REG_INTERFACE_CTL1		0x06
#define		CS43L22_REG_INTERFACE_CTL2		0x07
#define		CS43L22_REG_PASSTHR_A_SELECT	0x08
#define		CS43L22_REG_PASSTHR_B_SELECT	0x09
#define		CS43L22_REG_ANALOG_ZC_SR_SETT	0x0A
#define		CS43L22_REG_PASSTHR_GANG_CTL	0x0C
#define		CS43L22_REG_PLAYBACK_CTL1		0x0D
#define		CS43L22_REG_MISC_CTL			0x0E
#define		CS43L22_REG_PLAYBACK_CTL2		0x0F
#define		CS43L22_REG_PASSTHR_A_VOL		0x14
#define		CS43L22_REG_PASSTHR_B_VOL		0x15
#define		CS43L22_REG_PCMA_VOL			0x1A
#define		CS43L22_REG_PCMB_VOL			0x1B
#define		CS43L22_REG_BEEP_FREQ_ON_TIME	0x1C
#define		CS43L22_REG_BEEP_VOL_OFF_TIME	0x1D
#define		CS43L22_REG_BEEP_TONE_CFG		0x1E
#define		CS43L22_REG_TONE_CTL			0x1F
#define		CS43L22_REG_MASTER_A_VOL		0x20
#define		CS43L22_REG_MASTER_B_VOL		0x21
#define		CS43L22_REG_HEADPHONE_A_VOL		0x22
#define		CS43L22_REG_HEADPHONE_B_VOL		0x23
#define		CS43L22_REG_SPEAKER_A_VOL		0x24
#define		CS43L22_REG_SPEAKER_B_VOL		0x25
#define		CS43L22_REG_CH_MIXER_SWAP		0x26
#define		CS43L22_REG_LIMIT_CTL1			0x27
#define		CS43L22_REG_LIMIT_CTL2			0x28
#define		CS43L22_REG_LIMIT_ATTACK_RATE	0x29
#define		CS43L22_REG_OVF_CLK_STATUS		0x2E
#define		CS43L22_REG_BATT_COMPENSATION	0x2F
#define		CS43L22_REG_VP_BATTERY_LEVEL	0x30
#define		CS43L22_REG_SPEAKER_STATUS		0x31
#define		CS43L22_REG_TEMPMONITOR_CTL		0x32
#define		CS43L22_REG_THERMAL_FOLDBACK	0x33
#define		CS43L22_REG_CHARGE_PUMP_FREQ	0x34

#define		CS43L22_ID						0xE0
#define		CS43L22_ID_MASK					0xF8
#define		CS43L22_CHIPID_ADDR				0x01


typedef enum
{
	BUFFER_OFFSET_NONE				= 0,
	BUFFER_OFFSET_HALF,
	BUFFER_OFFSET_FULL,

}	qePCM_BufferOffset_t;


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *												General Peripheral Class
 */
class qcCS43L22 : virtual public qcPeripheral
{
	private :

		ui32				currentSampleRate;


	public :

		qcCS43L22			(void)
							{
								currentSampleRate = qcfgINITIAL_SAMPLE_RATE;
							}

		qePCM_BufferOffset_t
							bufferOffset;

		ui8					volume;


		void				DAC_Init (void);
		void				DAC_Reset (void);

		void				DAC_Write (ui8 reg, ui8 value);

		void				DAC_SetMute (ui32 cmd);
		ui8					DAC_ConvertVolume (ui8 volume) { return (volume > 100) ? 255 : ((volume * 255) / 100); }
		void				DAC_SetVolume (ui8 volume);

		void				DAC_SetSampleRate (ui32 sampleRate);

};




#define I2S_STANDARD                  I2S_STANDARD_PHILIPS



/* Audio status definition */

#define AUDIO_OK                        0

#define AUDIO_ERROR                     1

#define AUDIO_TIMEOUT                   2



/* Position in the audio play buffer */

//__IO BUFFER_StateTypeDef buffer_offset = BUFFER_OFFSET_NONE;



/* Codec output DEVICE */

#define OUTPUT_DEVICE_SPEAKER         1

#define OUTPUT_DEVICE_HEADPHONE       2

#define OUTPUT_DEVICE_BOTH            3

#define OUTPUT_DEVICE_AUTO            4



/* MUTE commands */

#define AUDIO_MUTE_ON                 1

#define AUDIO_MUTE_OFF                0



/* Defines for the Audio playing process */

#define PAUSE_STATUS     ((uint32_t)0x00) /* Audio Player in Pause Status */

#define RESUME_STATUS    ((uint32_t)0x01) /* Audio Player in Resume Status */

#define IDLE_STATUS      ((uint32_t)0x02) /* Audio Player in Idle Status */





#define AUDIO_RESET_GPIO_CLK_ENABLE()         __GPIOD_CLK_ENABLE()

#define AUDIO_RESET_PIN                       GPIO_PIN_4

#define AUDIO_RESET_GPIO                      GPIOD



#define CODEC_STANDARD                0x04

#define   CS43L22_REG_MISC_CTL            0x0E



#define AUDIO_I2C_ADDRESS                     0x94

#define CS43L22_CHIPID_ADDR    0x01

#define  CS43L22_ID            0xE0

#define  CS43L22_ID_MASK       0xF8



#define PCM_BUFFER_SIZE		(4096 * 2)  // todo



#define AUDIODATA_SIZE                  2   /* 16-bits audio data size */


/*  = = = = = = = = = = = = = = = = = = = = = = = */

#endif		/* OUTPUTS_CS43L22_H_ */
