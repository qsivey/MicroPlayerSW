/** ____________________________________________________________________
 *
 *	@file		analogInput.cpp
 *
 *				BaseCamElectronics Team
 *
 *				Mini-Remote Controller project
 *
 *				https://www.basecamelectronics.com
 *  ____________________________________________________________________
 */

#include "analogChannels.h"


#include "st7789.h"
#include "font_test.h"

/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   Class Methods
 */
void qcAnalogChannels::ADC_Init (void)
{
	ADC_DMA_CLOCK_ENABLE();

	ADC_DMA_HANDLE.Instance = ADC_DMA_STREAM;

	ADC_DMA_HANDLE.Init.Channel					= ADC_DMA_CHANNEL;
	ADC_DMA_HANDLE.Init.FIFOMode				= DMA_FIFOMODE_DISABLE;
	ADC_DMA_HANDLE.Init.FIFOThreshold			= DMA_FIFO_THRESHOLD_HALFFULL;
	ADC_DMA_HANDLE.Init.MemBurst				= DMA_MBURST_SINGLE;
	ADC_DMA_HANDLE.Init.PeriphBurst				= DMA_PBURST_SINGLE;
	ADC_DMA_HANDLE.Init.Direction				= DMA_PERIPH_TO_MEMORY;
	ADC_DMA_HANDLE.Init.PeriphInc				= DMA_PINC_DISABLE;
	ADC_DMA_HANDLE.Init.MemInc					= DMA_MINC_ENABLE;
	ADC_DMA_HANDLE.Init.PeriphDataAlignment		= DMA_PDATAALIGN_HALFWORD;
	ADC_DMA_HANDLE.Init.MemDataAlignment		= DMA_MDATAALIGN_HALFWORD;
	ADC_DMA_HANDLE.Init.Mode					= DMA_CIRCULAR;
	ADC_DMA_HANDLE.Init.Priority				= DMA_PRIORITY_HIGH;

	if (HAL_DMA_Init(&ADC_DMA_HANDLE) != HAL_OK)
		HardwareErrorHandler();

	/* Associate the initialized DMA handle to the ADC handle */
	__HAL_LINKDMA(&ADC_HANDLE, DMA_Handle, ADC_DMA_HANDLE);

	HAL_NVIC_SetPriority(ADC_DMA_CH_IRQN, 0, 0);
	HAL_NVIC_EnableIRQ(ADC_DMA_CH_IRQN);

	ADC_CLOCK_ENABLE();

	ADC_HANDLE.Instance = ADC_INSTANCE;

	ADC_HANDLE.Init.ClockPrescaler			= ADC_CLOCKPRESCALER_PCLK_DIV2;
	ADC_HANDLE.Init.Resolution				= ADC_RESOLUTION_12B;
	ADC_HANDLE.Init.ExternalTrigConvEdge	= ADC_EXTERNALTRIGCONVEDGE_NONE;
	ADC_HANDLE.Init.ExternalTrigConv		= ADC_SOFTWARE_START;
	ADC_HANDLE.Init.ContinuousConvMode		= ENABLE;
	ADC_HANDLE.Init.DiscontinuousConvMode	= DISABLE;
	ADC_HANDLE.Init.DMAContinuousRequests	= ENABLE;
	ADC_HANDLE.Init.EOCSelection			= ADC_EOC_SEQ_CONV;
	ADC_HANDLE.Init.ScanConvMode			= ENABLE;
	ADC_HANDLE.Init.NbrOfDiscConversion		= 0;
	ADC_HANDLE.Init.DataAlign				= ADC_DATAALIGN_RIGHT;
	ADC_HANDLE.Init.NbrOfConversion			= 1;

	if (HAL_ADC_Init(&ADC_HANDLE) != HAL_OK)
		HardwareErrorHandler();

	ADC_ChannelConfTypeDef sConfig = { 0 };

	sConfig.SamplingTime	= ADC_SAMPLETIME_144CYCLES;
	sConfig.Channel			= BAT_LEVEL_ADC_CHANNEL;
	sConfig.Rank			= 1;

	if (HAL_ADC_ConfigChannel(&ADC_HANDLE, &sConfig) != HAL_OK)
		HardwareErrorHandler();

	/* ADC Channel GPIO pin configuration */
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	GPIO_InitStruct.Pin = BAT_LEVEL_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	HAL_GPIO_Init(BAT_LEVEL_GPIO_PORT, &GPIO_InitStruct);

	if (HAL_ADC_Start_DMA(&ADC_HANDLE, (ui32*)analogValues, ANALOG_VALUES_AVERAGE) != HAL_OK)
		HardwareErrorHandler();
}


void qcAnalogChannels::ReadBatteryLevel (void)
{
	ui32 averageValue = 0;

	for (ui8 i = 0; i < ANALOG_VALUES_AVERAGE; i++)
		averageValue += analogValues[i];

	batteryLevel = averageValue / ANALOG_VALUES_AVERAGE;
}
