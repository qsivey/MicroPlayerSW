/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#ifndef		HARDWARE_STM32_IT_H_
#define 	HARDWARE_STM32_IT_H_

#ifdef		__cplusplus
extern		"C" {
#endif
/*  = = = = = = = = = = = = = = = = = = = = = = = */


#include	"microPlayer.h"


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 * 								 Function Prototypes
 */
void NMI_Handler (void);
void HardFault_Handler (void);
void MemManage_Handler (void);
void BusFault_Handler (void);
void UsageFault_Handler (void);
void DebugMon_Handler (void);
#if (qcfgPROJ_USE_FREERTOS == setOFF)
	void SVC_Handler (void);
	void PendSV_Handler (void);
	void SysTick_Handler (void);
#endif

void VOLUME_DOWN_IRQ_HANDLER (void);
void VOLUME_UP_IRQ_HANDLER (void);
void TRACK_PLAY_IRQ_HANDLER (void);
void TRACK_BACK_IRQ_HANDLER (void);
void TRACK_NEXT_IRQ_HANDLER (void);

void SDMMC1_IRQHandler (void);
void SPI1_IRQHandler (void);
void DAC_I2S_TX_DMA_IRQ_HANDLER (void);
void SPI3_IRQHandler (void);
void LCD_SPI_TX_DMA_IRQ_HANDLER (void);


/*  = = = = = = = = = = = = = = = = = = = = = = = */
#ifdef 		__cplusplus
			}
#endif

#endif		/* HARDWARE_STM32_IT_H_ */
