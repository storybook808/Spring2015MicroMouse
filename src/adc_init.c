/*
 * adc_init.c
 *
 *  Created on: Nov 20, 2014
 *      Author: Steven Chen
 */

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_adc.h"
#include "adc_init.h"

void adc_init()
{
	GPIO_InitTypeDef      GPIO_InitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	DMA_InitTypeDef       DMA_InitStruct;

	/* Enable ADC3, DMA2 and GPIO clocks ****************************************/
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

//	/* DMA2 Stream0 channel0 configuration **************************************/
//	DMA_InitStruct.DMA_Channel = DMA_Channel_0;
//	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;//ADC1's data register
//	DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)&ADC1ConvertedValue;
//	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
//	DMA_InitStruct.DMA_BufferSize = 2;
//	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//Reads 16 bit values
//	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//Stores 16 bit values
//	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
//	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
//	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
//	DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
//	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//	DMA_Init(DMA2_Stream0, &DMA_InitStruct);
//	DMA_Cmd(DMA2_Stream0, ENABLE);

	//GPIOA configuration: center left detector
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//GPIOB configuration: voltage reader
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* ADC Common Init **********************************************************/
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);


	/* ADC1 Init ****************************************************************/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);


	/* ADC1 regular channel7 configuration *************************************/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7 , 1, ADC_SampleTime_3Cycles);

	/* ADC1 regular channel9 configuration *************************************/
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_9 , 2, ADC_SampleTime_3Cycles);

//	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* Start ADC1 Software Conversion */
	ADC_SoftwareStartConv(ADC1);
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
}
