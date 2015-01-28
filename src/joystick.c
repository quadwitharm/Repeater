#include "joystick.h"

ADC_HandleTypeDef    AdcHandle;
ADC_ChannelConfTypeDef sConfig[2];

bool JOYSTICK_init(){

    /*##-1- Configure the ADC peripheral #######################################*/
    AdcHandle.Instance          = ADCx;

    AdcHandle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
    AdcHandle.Init.Resolution = ADC_RESOLUTION12b;
    AdcHandle.Init.ScanConvMode = DISABLE;
    AdcHandle.Init.ContinuousConvMode = ENABLE;
    AdcHandle.Init.DiscontinuousConvMode = DISABLE;
    AdcHandle.Init.NbrOfDiscConversion = 0;
    AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    AdcHandle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
    AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    AdcHandle.Init.NbrOfConversion = 1;
    AdcHandle.Init.DMAContinuousRequests = ENABLE;
    AdcHandle.Init.EOCSelection = DISABLE;

    if(HAL_ADC_Init(&AdcHandle) != HAL_OK)
    {
        /* Initialization Error */
        return false;
    }

    sConfig[0].Channel = ADCx_CHANNEL_1;
    sConfig[0].Rank = 1;
    sConfig[0].SamplingTime = ADC_SAMPLETIME_3CYCLES;
    sConfig[0].Offset = 0;

    sConfig[1].Channel = ADCx_CHANNEL_2;
    sConfig[1].Rank = 1;
    sConfig[1].SamplingTime = ADC_SAMPLETIME_3CYCLES;
    sConfig[1].Offset = 0;
    return true;
}

void ADC_SwitchChannel(int channel){
    HAL_ADC_ConfigChannel(&AdcHandle, &sConfig[channel]);
    HAL_ADC_Start(&AdcHandle);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
    /* Turn LED3 on: Transfer process is correct */
    
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    /* Enable GPIO clock */

    ADCx_CHANNEL_GPIO_CLK_ENABLE();
    /* ADC3 Periph clock enable */
    ADCx_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    GPIO_InitStruct.Pin = ADCx_CHANNEL_PIN_1 | ADCx_CHANNEL_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADCx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);
}

/**
 * @brief ADC MSP De-Initialization
 *        This function frees the hardware resources used in this example:
 *          - Disable the Peripheral's clock
 *          - Revert GPIO to their default state
 * @param hadc: ADC handle pointer
 * @retval None
 */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
    /*##-1- Reset peripherals ##################################################*/
    ADCx_FORCE_RESET();
    ADCx_RELEASE_RESET();

    /*##-2- Disable peripherals and GPIO Clocks ################################*/
    HAL_GPIO_DeInit(ADCx_CHANNEL_GPIO_PORT, ADCx_CHANNEL_PIN_1);
    HAL_GPIO_DeInit(ADCx_CHANNEL_GPIO_PORT, ADCx_CHANNEL_PIN_2);
}
