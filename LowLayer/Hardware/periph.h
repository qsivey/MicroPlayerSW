/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */
/*	Note:
 * 		- See DMAx request mapping for streams and channels in reference
 * 		manual;
 * 		- See STM32x alternate functions in datasheet
 */

#ifndef		HARDWARE_PERIPH_MAP_H_
#define		HARDWARE_PERIPH_MAP_H_

/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */


#include	"initialConfig.h"
#include	"projectConfig.h"


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *													   ADC Configuration
 */
/* -------------------------------------------------
 * ADC Module Config
 */
#define		ADC_HANDLE				qadc1
#define		ADC_INSTANCE			ADC1

#define		ADC_CLOCK_ENABLE()		__HAL_RCC_ADC_CLK_ENABLE()

/* -------------------------------------------------
 * ADC Channels Config
 */
/* Battery level channel */
#define		BAT_LEVEL_ADC_CHANNEL	ADC_CHANNEL_0

#define		BAT_LEVEL_GPIO_PORT		GPIOA
#define		BAT_LEVEL_PIN			GPIO_PIN_0


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *													   DAC Configuration
 */
/* -------------------------------------------------
 * DAC I2S GPIO Config
 */
/* DAC I2S SD pin */
#define		DAC_I2S_SD_GPIO_PORT	GPIOA
#define		DAC_I2S_SD_PIN			GPIO_PIN_7
#define		DAC_I2S_SD_PIN_AF		GPIO_AF5_SPI1

/* DAC I2S CK pin */
#define		DAC_I2S_CK_GPIO_PORT	GPIOA
#define		DAC_I2S_CK_PIN			GPIO_PIN_5
#define		DAC_I2S_CK_PIN_AF		GPIO_AF5_SPI1

/* DAC I2S WS pin */
#define		DAC_I2S_WS_GPIO_PORT	GPIOA
#define		DAC_I2S_WS_PIN			GPIO_PIN_4
#define		DAC_I2S_WS_PIN_AF		GPIO_AF5_SPI1

/* DAC I2S MCK pin */
#define		DAC_I2S_MCK_GPIO_PORT	GPIOC
#define		DAC_I2S_MCK_PIN			GPIO_PIN_4
#define		DAC_I2S_MCK_PIN_AF		GPIO_AF5_SPI1

#define		DAC_I2S_GPIO_CLOCK_ENABLE()		__HAL_RCC_GPIOA_CLK_ENABLE();\
											__HAL_RCC_GPIOC_CLK_ENABLE();

/* -------------------------------------------------
 * DAC I2S Module Config
 */
#define		DAC_I2S_HANDLE			qi2s1
#define		DAC_I2S_INSTANCE		SPI1

#define		DAC_I2S_CLOCK_ENABLE()	__HAL_RCC_SPI1_CLK_ENABLE()

/* -------------------------------------------------
 * DAC I2S DMA Module Config
 */
#define		DAC_I2S_TX_DMA_HANDLE	qdma_spi1_tx
#define		DAC_I2S_TX_DMA_INSTANCE	GPDMA1_Channel5
#define		DAC_I2S_TX_DMA_REQUEST	GPDMA1_REQUEST_SPI1_TX

#define		DAC_I2S_TX_DMA_NODE		qNode_GPDMA1_Channel5
#define		DAC_I2S_TX_DMA_LIST		qList_GPDMA1_Channel5

#define 	DAC_I2S_TX_DMA_CH_IRQN			GPDMA1_Channel5_IRQn
#define		DAC_I2S_TX_DMA_IRQ_HANDLER		GPDMA1_Channel5_IRQHandler

#define		DAC_I2S_TX_DMA_NODE_HANDLE		qdma_spi1_tx_node

#define		DAC_I2S_DMA_CLOCK_ENABLE()		__HAL_RCC_GPDMA1_CLK_ENABLE()

/* -------------------------------------------------
 * DAC I2C GPIO Config
 */
/* DAC I2C SDA pin */
#define		DAC_I2C_SDA_PIN			GPIO_PIN_7
#define		DAC_I2C_SDA_GPIO_PORT	GPIOB

/* DAC I2C SCL pin */
#define		DAC_I2C_SCL_GPIO_PORT	GPIOB
#define		DAC_I2C_SCL_PIN			GPIO_PIN_6

#define		DAC_I2C_PINS_AF			GPIO_AF4_I2C1

#define		DAC_I2C_GPIO_CLOCK_ENABLE()		__HAL_RCC_GPIOB_CLK_ENABLE()

/* -------------------------------------------------
 * DAC I2C Module Config
 */
#define		DAC_I2C_HANDLE			qi2c1
#define		DAC_I2C_INSTANCE		I2C1

#define		DAC_I2C_CLOCK_ENABLE()	__HAL_RCC_I2C1_CLK_ENABLE()

/* -------------------------------------------------
 * DAC Other GPIO Config
 */
#define		DAC_XSMT_GPIO_PORT		GPIOA
#define		DAC_XSMT_PIN			GPIO_PIN_3


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *												   SD Card Configuration
 */
// todo


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *													   LCD Configuration
 */
/* -------------------------------------------------
 * LCD SPI GPIO Config
 */
/* LCD SPI MOSI pin */
#define		LCD_SPI_MOSI_GPIO_PORT	GPIOB
#define		LCD_SPI_MOSI_PIN		GPIO_PIN_5
#define		LCD_SPI_MOSI_PIN_AF		GPIO_AF7_SPI3

/* LCD SPI SCL pin */
#define		LCD_SPI_SCL_GPIO_PORT	GPIOB
#define		LCD_SPI_SCL_PIN			GPIO_PIN_3
#define		LCD_SPI_SCL_PIN_AF		GPIO_AF6_SPI3

#define		LCD_SPI_GPIO_CLOCK_ENABLE()		__HAL_RCC_GPIOB_CLK_ENABLE()

/* -------------------------------------------------
 * LCD SPI Module Config
 */
#define		LCD_SPI_HANDLE			qspi3
#define		LCD_SPI_INSTANCE		SPI3

#define		LCD_SPI_CLOCK_ENABLE()	__HAL_RCC_SPI3_CLK_ENABLE()

/* -------------------------------------------------
 * LCD Other Pins Config
 */
/* LCD SPI CS pin */
#define		LCD_SPI_CS_GPIO_PORT	GPIOA
#define		LCD_SPI_CS_PIN			GPIO_PIN_8

/* LCD SPI DC pin */
#define		LCD_SPI_DC_GPIO_PORT	GPIOA
#define		LCD_SPI_DC_PIN			GPIO_PIN_9

/* LCD SPI RES pin */
#define		LCD_SPI_RES_GPIO_PORT	GPIOA
#define		LCD_SPI_RES_PIN			GPIO_PIN_10

/* -------------------------------------------------
 * LCD SPI DMA Module Config
 */
#define		LCD_SPI_TX_DMA_HANDLE	qdma_spi3_tx
#define		LCD_SPI_TX_DMA_INSTANCE	GPDMA1_Channel7
#define		LCD_SPI_TX_DMA_REQUEST	GPDMA1_REQUEST_SPI3_TX

#define 	LCD_SPI_TX_DMA_CH_IRQN			GPDMA1_Channel7_IRQn
#define		LCD_SPI_TX_DMA_IRQ_HANDLER		GPDMA1_Channel7_IRQHandler

#define		LCD_SPI_DMA_CLOCK_ENABLE()		__HAL_RCC_GPDMA1_CLK_ENABLE()


/* -------------------------------------------------
 * LCD TIM Module Config
 */
#define		LCD_TIM_HANDLE			qtim2
#define		LCD_TIM_INSTANCE		TIM2

#define		LCD_TIM_CHANNEL			TIM_CHANNEL_2

#define		LCD_TIM_CLOCK_ENABLE()	__HAL_RCC_TIM2_CLK_ENABLE()

/* -------------------------------------------------
 * LCD TIM Module GPIO Config
 */
#define		LCD_TIM_CH_GPIO_PORT	GPIOA
#define		LCD_TIM_CH_PIN			GPIO_PIN_1
#define		LCD_TIM_CH_PIN_AF		GPIO_AF1_TIM2


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														Digital Channels
 */
/* Buttons common */
#define		BUTTONS_GPIO_PORT				GPIOC
#define		BUTTONS_GPIO_CLOCK_ENABLE()		__HAL_RCC_GPIOC_CLK_ENABLE()

/* Volume down pin */
#define 	VOLUME_DOWN_PIN			GPIO_PIN_1
#define		VOLUME_DOWN_IRQN		EXTI1_IRQn
#define		VOLUME_DOWN_IRQ_HANDLER	EXTI1_IRQHandler

/* Volume up pin */
#define 	VOLUME_UP_PIN			GPIO_PIN_2
#define		VOLUME_UP_IRQN			EXTI2_IRQn
#define		VOLUME_UP_IRQ_HANDLER	EXTI2_IRQHandler

/* Back pin */
#define 	TRACK_BACK_PIN			GPIO_PIN_14
#define		TRACK_BACK_IRQN			EXTI14_IRQn
#define		TRACK_BACK_IRQ_HANDLER	EXTI14_IRQHandler

/* Next pin */
#define 	TRACK_NEXT_PIN			GPIO_PIN_15
#define		TRACK_NEXT_IRQN			EXTI15_IRQn
#define		TRACK_NEXT_IRQ_HANDLER	EXTI15_IRQHandler

/* Pause pin */
#define 	TRACK_PLAY_PIN			GPIO_PIN_13
#define		TRACK_PLAY_IRQN			EXTI13_IRQn
#define		TRACK_PLAY_IRQ_HANDLER	EXTI13_IRQHandler


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *												General Peripheral Class
 */
class qcPeripheral
{
	public:

		/* RCC Peripheral Clock objects */
		RCC_PeriphCLKInitTypeDef
							PeriphClkInitStruct;

		/* ADC handle objects */
		ADC_HandleTypeDef	ADC_HANDLE;

		/* DAC handle objects */
		I2S_HandleTypeDef	DAC_I2S_HANDLE;
		DMA_HandleTypeDef	DAC_I2S_TX_DMA_HANDLE;
		DMA_NodeTypeDef		DAC_I2S_TX_DMA_NODE;
		DMA_QListTypeDef	DAC_I2S_TX_DMA_LIST;
		I2C_HandleTypeDef	DAC_I2C_HANDLE;

		/* LCD handle objects */
		SPI_HandleTypeDef	LCD_SPI_HANDLE;
		TIM_HandleTypeDef   LCD_TIM_HANDLE;
		DMA_HandleTypeDef	LCD_SPI_TX_DMA_HANDLE;


		/* Peripheral methods */
		void				SystemClockConfig (void);
		void				PeriphCommonClockConfig (void);

		void				AllGPIO_ClockEnable (void);
		void				AllGPIO_ClockDisable (void);

};


NORETURN__ void HardwareErrorHandler (void);
NORETURN__ void SoftwareErrorHandler (void);


/* = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = */

#endif		/* HARDWARE_PERIPH_MAP_H_ */
