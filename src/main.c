/**
*****************************************************************************
**
**  File        : main.c
**
**  Abstract    : main function.
**
**  Functions   : main
**
**  Environment : Atollic TrueSTUDIO(R)
**                STMicroelectronics STM32F4xx Standard Peripherals Library
**
**  Distribution: The file is distributed “as is,” without any warranty
**                of any kind.
**
**  (c)Copyright Atollic AB.
**  You may use this file as-is or modify it according to the needs of your
**  project. This file may only be built (assembled or compiled and linked)
**  using the Atollic TrueSTUDIO(R) product. The use of this file together
**  with other tools than Atollic TrueSTUDIO(R) is not permitted.
**
*****************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_adc.h"
#include "gpio_init.h"
#include "pwm_init.h"
#include "adc_init.h"

/* Private defines */


/* Private variables */
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef       TIM_OCInitStructure;

uint16_t CCRR_Val = 0;
uint16_t CCRL_Val = 0;
uint16_t Buzzer_Val = 0;

volatile __IO uint16_t ADC1ConvertedValue[5] = {0, 0, 0, 0, 0};
__IO uint32_t          ADC1ConvertedVoltage = 0;
__IO uint32_t          battery_level = 0;
__IO uint32_t          sensor_level[4] = {0, 0, 0, 0};

/* Private function prototypes */
void delay_random();

/**
 **===========================================================================
 **
 **  Abstract: main program
 **
 **===========================================================================
 **/
int main(void)
{

	/**
	 **=======================================================================
	 ** Motor [Right]
	 ** PB8 : Speed     : TIM4 CH3
	 ** PB9 : Direction : TIM4 CH4
	 ** PA0 : Encoder A : TIM2 CH2 TIM5 CH1
	 ** PA1 : Encoder B : TIM2 CH1 TIM5 CH2
	 **=======================================================================
	 ** Motor [Left]
	 ** PB6 : Speed     : TIM4 CH1
	 ** PB7 : Direction : TIM4 CH2
	 ** PA15: Encoder A : TIM2 CH1
	 ** PB3 : Encoder B : TIM2 CH2
	 **=======================================================================
	 ** Buzzer
	 ** PB4 :           : TIM3 CH1
	 **=======================================================================
	 ** LED
	 ** PC0 : Front
	 ** PB11:
	 ** PB10:
	 ** PA5 :
	 ** PA6 : Back
	 **=======================================================================
	 ** Infrared Range Finder [Left]
	 ** PH6 : Emitter
	 ** PB0 : Detector	: ADC12 IN8
	 **=======================================================================
	 ** Infrared Range Finder [Center Left]
	 ** PC15: Emitter
	 ** PA7 : Detector	: ADC12 IN7
	 **=======================================================================
	 ** Infrared Range Finder [Center Right]
	 ** PH5 : Emitter
	 ** PA4 : Detector	: ADC12 IN4
	 **=======================================================================
	 ** Infrared Range Finder [Right]
	 ** PC14: Emitter
	 ** PC5 : Detector	: ADC12 IN15
	 **=======================================================================
	 ** Voltage Detector // Go off @ 3.1 V
	 ** PB1 :           : ADC12 IN9
	 **=======================================================================
	 **/

	int x;

	gpio_init();

	pwm_init();

	//adc_init();

	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	DMA_InitTypeDef       DMA_InitStructure;

	/* Enable peripheral clocks *************************************************/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	/* DMA2_Stream0 channel0 configuration **************************************/
	DMA_DeInit(DMA2_Stream0);
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC1ConvertedValue;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 5;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
	/* DMA2_Stream0 enable */
	DMA_Cmd(DMA2_Stream0, ENABLE);

	/* ADC Common Init **********************************************************/
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC1 Init ****************************************************************/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 5;
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC1 regular channel8 configuration *************************************/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_3Cycles);

	/* ADC1 regular channel7 configuration *************************************/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 2, ADC_SampleTime_3Cycles);


	/* ADC1 regular channel4 configuration *************************************/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 3, ADC_SampleTime_3Cycles);

	/* ADC1 regular channel15 configuration *************************************/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 4, ADC_SampleTime_3Cycles);

	/* ADC1 regular channel9 configuration *************************************/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 5, ADC_SampleTime_3Cycles);

	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* Enable ADC1 **************************************************************/
	ADC_Cmd(ADC1, ENABLE);

	/* Start ADC1 Software Conversion */
	ADC_SoftwareStartConv(ADC1);

	// Standard TIM configurations
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	// Turn on emitters
	GPIO_ResetBits(GPIOH, GPIO_Pin_6);
	GPIO_SetBits(GPIOC, GPIO_Pin_15);
	GPIO_ResetBits(GPIOH, GPIO_Pin_5);
	GPIO_ResetBits(GPIOC, GPIO_Pin_14);

	do
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_11);  // Turn on blue LED
		sensor_level[1] = ADC1ConvertedValue[1] *3.3/0xFFF;
	}while(sensor_level[1] <= 1.0);

	GPIO_ResetBits(GPIOB, GPIO_Pin_11); //Turn off blue LED

	for(x = 0; x < 5; x++)
	{
		delay_random();
	}
	GPIO_SetBits(GPIOA, GPIO_Pin_6);

	delay_random();

	GPIO_SetBits(GPIOA, GPIO_Pin_5);
	delay_random();
	GPIO_SetBits(GPIOB, GPIO_Pin_10);
	delay_random();
	GPIO_SetBits(GPIOB, GPIO_Pin_11);


	delay_random();
	GPIO_SetBits(GPIOC, GPIO_Pin_0);
	delay_random();
	GPIO_ResetBits(GPIOC, GPIO_Pin_0);
	GPIO_ResetBits(GPIOB, GPIO_Pin_11);
	GPIO_ResetBits(GPIOB, GPIO_Pin_10);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	GPIO_ResetBits(GPIOA, GPIO_Pin_6);
	delay_random();
	GPIO_SetBits(GPIOC, GPIO_Pin_0);
	GPIO_SetBits(GPIOB, GPIO_Pin_11);
	GPIO_SetBits(GPIOB, GPIO_Pin_10);
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
	GPIO_SetBits(GPIOA, GPIO_Pin_6);
	delay_random();
	GPIO_ResetBits(GPIOC, GPIO_Pin_0);
	GPIO_ResetBits(GPIOB, GPIO_Pin_11);
	GPIO_ResetBits(GPIOB, GPIO_Pin_10);
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
	GPIO_ResetBits(GPIOA, GPIO_Pin_6);

	// Turn on emitters
	GPIO_ResetBits(GPIOH, GPIO_Pin_6);
	GPIO_SetBits(GPIOC, GPIO_Pin_15);
	GPIO_ResetBits(GPIOH, GPIO_Pin_5);
	GPIO_ResetBits(GPIOC, GPIO_Pin_14);

	while (1)
	{
		sensor_level[0] = ADC1ConvertedValue[0] *3.3/0xFFF;
		sensor_level[1] = ADC1ConvertedValue[1] *3.3/0xFFF;
		sensor_level[2] = ADC1ConvertedValue[2] *3.3/0xFFF;
		sensor_level[3] = ADC1ConvertedValue[3] *3.3/0xFFF;
		battery_level   = ADC1ConvertedValue[4] *3.3/0xFFF;

		if(sensor_level[1] >=1.0 /*GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)*/)
		{
			GPIO_SetBits(GPIOC, GPIO_Pin_0); // Turn on  LED

			CCRR_Val = 0;
			CCRL_Val = 0;
			Buzzer_Val = 0;
		}
		else
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_0); // Turn off  LED

			GPIO_SetBits(GPIOB, GPIO_Pin_9); // Right direction bit
			GPIO_ResetBits(GPIOB, GPIO_Pin_7); // Left direction bit

			CCRR_Val = 100;
			CCRL_Val = 100;
			Buzzer_Val = 111;
		}
//
//		if(sensor_level[0]>=1.0)
//		{
//			GPIO_SetBits(GPIOB, GPIO_Pin_11);  // Turn on blue LED
//		}
//		else
//		{
//			GPIO_ResetBits(GPIOB, GPIO_Pin_11); //Turn off blue LED
//		}
//
//		if(sensor_level[1]>=1.0)
//		{
//			GPIO_SetBits(GPIOB, GPIO_Pin_10);  // Turn on yellow LED
//		}
//		else
//		{
//			GPIO_ResetBits(GPIOB, GPIO_Pin_10); // Turn off yellow LED
//		}
//
//		if(battery_level > 2.0)
//		{
//			GPIO_SetBits(GPIOA, GPIO_Pin_6); // Turn on red LED
//		}
//		else
//		{
//			GPIO_ResetBits(GPIOA, GPIO_Pin_6); // Turn off LED
//		}
//
		if(CCRL_Val < 50)
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_7); // Left direction bit
//			GPIO_SetBits(GPIOB, GPIO_Pin_11);  // Turn on blue LED
		}
		else
		{
//			GPIO_ResetBits(GPIOB, GPIO_Pin_11); //Turn off blue LED
		}


		if(CCRR_Val < 50)
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_9); // Right direction bit
//			GPIO_SetBits(GPIOB, GPIO_Pin_10);  // Turn on yellow LED
		}
		else
		{
//			GPIO_ResetBits(GPIOB, GPIO_Pin_10); // Turn off yellow LED
		}

		/* PWM1 Mode configuration: Channel1 */
		TIM_OCInitStructure.TIM_Pulse = CCRL_Val;
		TIM_OC1Init(TIM4, &TIM_OCInitStructure);

		/* PWM1 Mode configuration: Channel3 */
		TIM_OCInitStructure.TIM_Pulse = CCRR_Val;
		TIM_OC3Init(TIM4, &TIM_OCInitStructure);

		/* PWM1 Mode configuration: TIM Channel1 */
		TIM_OCInitStructure.TIM_Pulse = Buzzer_Val;
		TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	}
}

void delay_random()
{
	int temp1, temp2;

	//keep program in stand-by with two large for-loops
	for(temp1 = 0; temp1 < 500; temp1++) for(temp2 = 0; temp2 < 500; temp2++);
}

