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


/* ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾
 *														   Class Methods
 */
void qcAnalogChannels::ADC_Init (void)
{
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

	sConfig.SamplingTime	= ADC_SAMPLETIME_92CYCLES_5;
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
