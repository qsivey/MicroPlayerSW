/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#ifndef		OUTPUTS_PCM5142_H_
#define		OUTPUTS_PCM5142_H_

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */


#include	"projectConfig.h"
#include	"periph.h"


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *													   PCM5142 Constants
 */
#define		PCM5142_I2C_ADDR		(0x4C << 1)


typedef enum
{
	BUFFER_OFFSET_NONE				= 0,
	BUFFER_OFFSET_HALF,
	BUFFER_OFFSET_FULL,

}	qePCM_BufferOffset_t;


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *												General Peripheral Class
 */
class qcPCM5142 : virtual public qcPeripheral
{
	private :

		ui32				currentSampleRate;
		ui8					currentBitRate;


	public :

		ui8					volume;
		bool				amplitudeBoost;

		qePCM_BufferOffset_t
							bufferOffset;


		void				PCM5142_Init (void);
		void				PCM5142_Reset (void);

		void				PCM5142_Write (ui8 reg, ui8 value);
		void				PCM5142_Read (ui8 reg, ui8 *value);

		void				PCM5142_Mute (void);
		void				PCM5142_Unmute (void);
		void				PCM5142_SetVolume (ui8 volume);

		void				PCM5142_SetBitRate (ui8 bitRate);
		void				PCM5142_SetSampleRate (ui32 sampleRate);

		void				PCM5142_I2S_IRQ_Handler (void);
		void				PCM5142_I2S_TxHalfCpltCallback (void) { bufferOffset = BUFFER_OFFSET_FULL; };
		void				PCM5142_I2S_TxHalfCallback (void) { bufferOffset = BUFFER_OFFSET_HALF; };

};


/*  = = = = = = = = = = = = = = = = = = = = = = = */

#endif		/* OUTPUTS_PCM5142_H_ */
