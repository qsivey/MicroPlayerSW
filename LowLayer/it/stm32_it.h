/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#ifndef		_STM32F7XX_IT_H_
#define 	_STM32F7XX_IT_H_

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
#if (PROJ_USE_FREERTOS == setOFF)
	void SVC_Handler (void);
	void PendSV_Handler (void);
	void SysTick_Handler (void);
#endif

void EXTI9_5_IRQHandler (void);
void EXTI15_10_IRQHandler (void);
void DAC_I2S_TX_DMA_IRQ_HANDLER (void);
void ADC_DMA_IRQ_HANDLER (void);

/*  = = = = = = = = = = = = = = = = = = = = = = = */
#ifdef 		__cplusplus
			}
#endif

#endif		/* _STM32F7XX_IT_H_ */
