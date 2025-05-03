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

#include	"initialConfig.h"
#include	"projectConfig.h"


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *													   ADC Configuration
 */
/* ADC Module Config
 */
#define		ADC_HANDLE				qadc1
#define		ADC_INSTANCE			ADC1

#define		ADC_CLOCK_ENABLE()		__HAL_RCC_ADC1_CLK_ENABLE()

/* -------------------------------------------------
 * ADC DMA Module Config
 */
#define		ADC_DMA_HANDLE	qdma2_adc
#define		ADC_DMA_STREAM	DMA2_Stream0
#define		ADC_DMA_CHANNEL	DMA_CHANNEL_0

#define 	ADC_DMA_CH_IRQN			DMA2_Stream0_IRQn
#define		ADC_DMA_IRQ_HANDLER		DMA2_Stream0_IRQHandler

#define		ADC_DMA_CLOCK_ENABLE()		__HAL_RCC_DMA2_CLK_ENABLE()

/* -------------------------------------------------
 * ADC Channels Config
 */
/* Battery level channel */
#define		BAT_LEVEL_ADC_CHANNEL	ADC_CHANNEL_2

#define		BAT_LEVEL_GPIO_PORT		GPIOA
#define		BAT_LEVEL_PIN			GPIO_PIN_2


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *													   DAC Configuration
 */
/* -------------------------------------------------
 * DAC I2S GPIO Config
 */
/* DAC I2S SD pin */
#define		DAC_I2S_SD_GPIO_PORT	GPIOB
#define		DAC_I2S_SD_PIN			GPIO_PIN_15
#define		DAC_I2S_SD_PIN_AF		GPIO_AF5_SPI2

/* DAC I2S CK pin */
#define		DAC_I2S_CK_GPIO_PORT	GPIOB
#define		DAC_I2S_CK_PIN			GPIO_PIN_13
#define		DAC_I2S_CK_PIN_AF		GPIO_AF5_SPI2

/* DAC I2S WS pin */
#define		DAC_I2S_WS_GPIO_PORT	GPIOB
#define		DAC_I2S_WS_PIN			GPIO_PIN_12
#define		DAC_I2S_WS_PIN_AF		GPIO_AF5_SPI2

/* DAC I2S MCK pin */
#define		DAC_I2S_MCK_GPIO_PORT	GPIOA
#define		DAC_I2S_MCK_PIN			GPIO_PIN_3
#define		DAC_I2S_MCK_PIN_AF		GPIO_AF5_SPI2

#define		DAC_I2S_GPIO_CLOCK_ENABLE()		__HAL_RCC_GPIOA_CLK_ENABLE();\
											__HAL_RCC_GPIOB_CLK_ENABLE();

/* -------------------------------------------------
 * DAC I2S Module Config
 */
#define		DAC_I2S_HANDLE			qi2s2
#define		DAC_I2S_INSTANCE		SPI2

#define		DAC_I2S_CLOCK_ENABLE()	__HAL_RCC_SPI2_CLK_ENABLE()

/* -------------------------------------------------
 * DAC I2C GPIO Config
 */
/* DAC I2C SDA pin */
#define		DAC_I2C_SDA_GPIO_PORT	GPIOB
#define		DAC_I2C_SDA_PIN			GPIO_PIN_7
#define		DAC_I2C_SDA_PIN_AF		GPIO_AF4_I2C1

/* DAC I2C SCL pin */
#define		DAC_I2C_SCL_GPIO_PORT	GPIOB
#define		DAC_I2C_SCL_PIN			GPIO_PIN_6
#define		DAC_I2C_SCL_PIN_AF		GPIO_AF4_I2C1

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
#define		DAC_RESET_GPIO_PORT		GPIOA
#define		DAC_RESET_PIN			GPIO_PIN_8

/* -------------------------------------------------
 * DAC I2S DMA Module Config
 */
#define		DAC_I2S_TX_DMA_HANDLE	qdma1_tx_spi2
#define		DAC_I2S_TX_DMA_STREAM	DMA1_Stream4
#define		DAC_I2S_TX_DMA_CHANNEL	DMA_CHANNEL_0

#define 	DAC_I2S_TX_DMA_CH_IRQN			DMA1_Stream4_IRQn
#define		DAC_I2S_TX_DMA_IRQ_HANDLER		DMA1_Stream4_IRQHandler

#define		DAC_I2S_DMA_CLOCK_ENABLE()		__HAL_RCC_DMA1_CLK_ENABLE()


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *												   SD Card Configuration
 */
/* -------------------------------------------------
 * SD SPI GPIO Config
 */
/* SD SPI MOSI pin */
#define		SD_SPI_MOSI_GPIO_PORT	GPIOB
#define		SD_SPI_MOSI_PIN			GPIO_PIN_5
#define		SD_SPI_MOSI_PIN_AF		GPIO_AF6_SPI3

/* SD SPI MISO pin */
#define		SD_SPI_MISO_GPIO_PORT	GPIOB
#define		SD_SPI_MISO_PIN			GPIO_PIN_4
#define		SD_SPI_MISO_PIN_AF		GPIO_AF6_SPI3

/* SD SPI SCL pin */
#define		SD_SPI_SCK_GPIO_PORT	GPIOB
#define		SD_SPI_SCK_PIN			GPIO_PIN_3
#define		SD_SPI_SCK_PIN_AF		GPIO_AF6_SPI3

#define		SD_SPI_GPIO_CLOCK_ENABLE()		__HAL_RCC_GPIOB_CLK_ENABLE();

/* -------------------------------------------------
 * SD SPI Module Config
 */
#define		SD_SPI_HANDLE			qspi3
#define		SD_SPI_INSTANCE			SPI3

#define		SD_SPI_CLOCK_ENABLE()	__HAL_RCC_SPI3_CLK_ENABLE()

/* -------------------------------------------------
 * SD Other GPIO Config
 */
/* SD SPI CS pin */
#define		SD_CS_GPIO_PORT			GPIOA
#define 	SD_CS_PIN				GPIO_PIN_9

#define		SD_GPIO_CLOCK_ENABLE()	__HAL_RCC_GPIOA_CLK_ENABLE();


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *													   LCD Configuration
 */
/* LCD SPI GPIO Config -------------------------
 */
/* LCD SPI MOSI pin */
#define		LCD_SPI_MOSI_GPIO_PORT	GPIOA
#define		LCD_SPI_MOSI_PIN		GPIO_PIN_7
#define		LCD_SPI_MOSI_PIN_AF		GPIO_AF5_SPI1

/* LCD SPI SCL pin */
#define		LCD_SPI_SCL_GPIO_PORT	GPIOA
#define		LCD_SPI_SCL_PIN			GPIO_PIN_5
#define		LCD_SPI_SCL_PIN_AF		GPIO_AF5_SPI1

#define		LCD_SPI_GPIO_CLOCK_ENABLE()		__HAL_RCC_GPIOA_CLK_ENABLE()

/* LCD SPI Module Config -----------------------
 */
#define		LCD_SPI_HANDLE			qspi1
#define		LCD_SPI_INSTANCE		SPI1

#define		LCD_SPI_CLOCK_ENABLE()	__HAL_RCC_SPI1_CLK_ENABLE()

/* LCD Other Pins Config -----------------------
 */
/* LCD SPI CS pin */
#define		LCD_SPI_CS_GPIO_PORT	GPIOB
#define		LCD_SPI_CS_PIN			GPIO_PIN_0

/* LCD SPI DC pin */
#define		LCD_SPI_DC_GPIO_PORT	GPIOB
#define		LCD_SPI_DC_PIN			GPIO_PIN_1

/* LCD SPI RES pin */
#define		LCD_SPI_RES_GPIO_PORT	GPIOA
#define		LCD_SPI_RES_PIN			GPIO_PIN_4

/* -------------------------------------------------
 * LCD SPI DMA Module Config
 */
#define		LCD_SPI_TX_DMA_HANDLE	qdma2_tx_spi2
#define		LCD_SPI_TX_DMA_STREAM	DMA2_Stream2
#define		LCD_SPI_TX_DMA_CHANNEL	DMA_CHANNEL_2

#define 	LCD_SPI_TX_DMA_CH_IRQN			DMA2_Stream2_IRQn
#define		LCD_SPI_TX_DMA_IRQ_HANDLER		DMA2_Stream2_IRQHandler

#define		LCD_SPI_DMA_CLOCK_ENABLE()		__HAL_RCC_DMA2_CLK_ENABLE()

/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														Digital Channels
 */
/* Volume down pin */
#define		VOLUME_DOWN_GPIO_PORT	GPIOB
#define 	VOLUME_DOWN_PIN			GPIO_PIN_9

/* Volume up pin */
#define		VOLUME_UP_GPIO_PORT		GPIOB
#define 	VOLUME_UP_PIN			GPIO_PIN_8

/* Back pin */
#define		TRACK_BACK_GPIO_PORT	GPIOC
#define 	TRACK_BACK_PIN			GPIO_PIN_14

/* Next pin */
#define		TRACK_NEXT_GPIO_PORT	GPIOC
#define 	TRACK_NEXT_PIN			GPIO_PIN_13

/* Pause pin */
#define		TRACK_PAUSE_GPIO_PORT	GPIOC
#define 	TRACK_PAUSE_PIN			GPIO_PIN_15


#define		DIGITAL_GPIO_CLOCK_ENABLE()		__HAL_RCC_GPIOB_CLK_ENABLE();\
											__HAL_RCC_GPIOC_CLK_ENABLE();


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *												General Peripheral Class
 */
class qcPeripheral
{
	public:

		/* ADC handle objects */
		ADC_HandleTypeDef	ADC_HANDLE;
		DMA_HandleTypeDef	ADC_DMA_HANDLE;

		/* DAC handle objects */
		I2S_HandleTypeDef	DAC_I2S_HANDLE;
		DMA_HandleTypeDef	DAC_I2S_TX_DMA_HANDLE;
		I2C_HandleTypeDef	DAC_I2C_HANDLE;

		/* LCD handle objects */
		SPI_HandleTypeDef	LCD_SPI_HANDLE;
		TIM_HandleTypeDef   htim2;
		DMA_HandleTypeDef	LCD_SPI_TX_DMA_HANDLE;

		bool				SPI_TxDMA_TC_Flag;

		/* SD card handle objects */
		SPI_HandleTypeDef	SD_SPI_HANDLE;

		/* Peripheral methods */
		void				SystemClockConfig (void);
		void				PeriphCommonClockConfig (void);

		void				AllGPIO_ClockEnable (void);
		void				AllGPIO_ClockDisable (void);

};


NORETURN__ void HardwareErrorHandler (void);
NORETURN__ void SoftwareErrorHandler (void);

#endif		/* HARDWARE_PERIPH_MAP_H_ */
