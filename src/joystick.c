#include "joystick.h"

ADC_HandleTypeDef    AdcHandle;

bool JOYSTICK_init(){
    ADC_ChannelConfTypeDef sConfig;

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

    /*##-2- Configure ADC regular channel ######################################*/
    sConfig.Channel = ADCx_CHANNEL;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    sConfig.Offset = 0;

    if(HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
    {
        /* Channel Configuration Error */
        return false;
    }
    return true;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle)
{
    /* Turn LED3 on: Transfer process is correct */
    
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef          GPIO_InitStruct;
    static DMA_HandleTypeDef  hdma_adc;

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO clock */
    ADCx_CHANNEL_GPIO_CLK_ENABLE();
    /* ADC3 Periph clock enable */
    ADCx_CLK_ENABLE();
    /* Enable DMA2 clock */
    DMAx_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* ADC3 Channel8 GPIO pin configuration */
    GPIO_InitStruct.Pin = ADCx_CHANNEL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADCx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);

    /*##-3- Configure the DMA streams ##########################################*/
    /* Set the parameters to be configured */
    hdma_adc.Instance = ADCx_DMA_STREAM;

    hdma_adc.Init.Channel  = ADCx_DMA_CHANNEL;
    hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_adc.Init.Mode = DMA_CIRCULAR;
    hdma_adc.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_adc.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    hdma_adc.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
    hdma_adc.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_adc.Init.PeriphBurst = DMA_PBURST_SINGLE;

    HAL_DMA_Init(&hdma_adc);

    /* Associate the initialized DMA handle to the the ADC handle */
    __HAL_LINKDMA(hadc, DMA_Handle, hdma_adc);

    /*##-4- Configure the NVIC for DMA #########################################*/
    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(ADCx_DMA_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADCx_DMA_IRQn);
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
    static DMA_HandleTypeDef  hdma_adc;

    /*##-1- Reset peripherals ##################################################*/
    ADCx_FORCE_RESET();
    ADCx_RELEASE_RESET();

    /*##-2- Disable peripherals and GPIO Clocks ################################*/
    /* De-initialize the ADC3 Channel8 GPIO pin */
    HAL_GPIO_DeInit(ADCx_CHANNEL_GPIO_PORT, ADCx_CHANNEL_PIN);

    /*##-3- Disable the DMA Streams ############################################*/
    /* De-Initialize the DMA Stream associate to transmission process */
    HAL_DMA_DeInit(&hdma_adc);

    /*##-4- Disable the NVIC for DMA ###########################################*/
    HAL_NVIC_DisableIRQ(ADCx_DMA_IRQn);
}
