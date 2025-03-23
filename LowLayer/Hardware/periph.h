/** ____________________________________________________________________
 *
 * 	MicroPlayer project
 *
 *	GitHub:		qsivey, Nik125Y
 *	Telegram:	@qsivey, @Nik125Y
 *	Email:		qsivey@gmail.com, topnikm@gmail.com
 *	____________________________________________________________________
 */

#ifndef		HARDWARE_PERIPH_MAP_H_
#define		HARDWARE_PERIPH_MAP_H_

#include	"initialConfig.h"


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   DAC Constants
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
 *														Digital Channels
 */



/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *												General Peripheral Class
 */
class qcPeripheral
{
	public:

		/* DAC handle objects */
		I2S_HandleTypeDef	DAC_I2S_HANDLE;
		DMA_HandleTypeDef	DAC_I2S_TX_DMA_HANDLE;
		I2C_HandleTypeDef	DAC_I2C_HANDLE;

		/* SD card handle objects */
		SPI_HandleTypeDef	SD_SPI_HANDLE;


		void				AllGPIO_ClockEnable (void);
		void				AllGPIO_ClockDisable (void);

};

//extern qcPeripheral_t Peripheral;


NORETURN__ void HardwareErrorHandler (void);
NORETURN__ void SoftwareErrorHandler (void);

#endif		/* HARDWARE_PERIPH_MAP_H_ */
