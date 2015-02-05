#include "joystick.h"
#include "send.h"

ADC_HandleTypeDef    AdcHandle;
ADC_ChannelConfTypeDef sConfig[] = {
    {.Channel = ADCx_CHANNEL_1, .Rank = 1, .SamplingTime = ADC_SAMPLETIME_3CYCLES },
    {.Channel = ADCx_CHANNEL_2, .Rank = 1, .SamplingTime = ADC_SAMPLETIME_3CYCLES },
    {.Channel = ADCx_CHANNEL_3, .Rank = 1, .SamplingTime = ADC_SAMPLETIME_3CYCLES },
    {.Channel = ADCx_CHANNEL_4, .Rank = 1, .SamplingTime = ADC_SAMPLETIME_3CYCLES },
};

bool JOYSTICK_init(){
    /*##-1- Configure the ADC peripheral ####################################*/
    AdcHandle.Instance = ADCx;

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

    if(HAL_ADC_Init(&AdcHandle) != HAL_OK){
        /* Initialization Error */
        return false;
    }
    return true;
}

void ADC_SwitchChannel(int channel){
    HAL_ADC_ConfigChannel(&AdcHandle, &sConfig[channel]);
    HAL_ADC_Start(&AdcHandle);
}

uint16_t ADC_ConversionPolling(int channel){
    ADC_SwitchChannel(channel);
    HAL_ADC_PollForConversion(&AdcHandle, 10000);
    if(HAL_ADC_GetState(&AdcHandle) == HAL_ADC_STATE_EOC_REG)
    {
        return HAL_ADC_GetValue(&AdcHandle);
    }
    /* Someing error */
    while(1);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* AdcHandle) {
    /* Currently use polling I/O, nothing to do */
}

/* TODO: dynamically estimate the middle value */
#define ROLL_MID_VALUE 1291
#define PITCH_MID_VALUE 2037
#define YAW_MID_VALUE 2037
#define ZSPEED_MID_VALUE 2037
#define ADC_MAX_VALUE 4095
#define ADC_MIN_VALUE 0
#define ROLL_MAX 20
#define PITCH_MAX 20
#define YAW_SPEED_FACTOR 100    // TODO: Test with hardware
#define ZSPEED_FACTOR 100
#define CENTER_AREA 0.03
#define SCALE_ADC_INPUT(IN, MID) \
   (IN > MID) ? \
       (float)(IN - MID) / (ADC_MAX_VALUE - MID) : \
       (float)(IN - MID) / (MID - ADC_MIN_VALUE)
#define FILTER_CENTER_AREA(value) \
    ((value > 0 ? value : -value) < CENTER_AREA) ? 0 : value

void JoystickRead_Task(){
    float yaw = 0, zSpeed = 0;
    while(1){
        uint16_t RollIn = ADC_ConversionPolling(0);
        uint16_t PitchIn= ADC_ConversionPolling(1);
        uint16_t YawIn = ADC_ConversionPolling(2);
        uint16_t zSpeedIn  = ADC_ConversionPolling(3);

        /* Setpoint of roll, pitch */
        float roll = SCALE_ADC_INPUT(RollIn, ROLL_MID_VALUE);
        float pitch = SCALE_ADC_INPUT(PitchIn, PITCH_MID_VALUE);

        float yawDelta = SCALE_ADC_INPUT(YawIn, YAW_MID_VALUE);
        float zSpeedDelta = SCALE_ADC_INPUT(zSpeedIn, ZSPEED_MID_VALUE);

        /* Center don't care zone */
        roll  = FILTER_CENTER_AREA(roll);
        pitch  = FILTER_CENTER_AREA(pitch);
        yawDelta = FILTER_CENTER_AREA(yawDelta);
        zSpeedDelta = FILTER_CENTER_AREA(zSpeedDelta);

        yaw += yawDelta * YAW_SPEED_FACTOR;
        zSpeed += zSpeedDelta * ZSPEED_FACTOR;

        // Temporary use gyro plot canvas to see the output
        float g[3] = { 0, roll * ROLL_MAX, pitch * PITCH_MAX};
        SendCommand_3( 0x01 , 0x00, (uint8_t *)g, 12);
    }
}

/**
 * @brief Initializate ADC hardware resources
 * @param hadc: ADC handle pointer
 * @retval None
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc){
    GPIO_InitTypeDef GPIO_InitStruct;
    /* Enable GPIO clock */
    ADCx_CHANNEL_GPIO_CLK_ENABLE();
    /* ADC3 Periph clock enable */
    ADCx_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO #######################################*/
    GPIO_InitStruct.Pin = ADCx_CHANNEL_PIN_1 | ADCx_CHANNEL_PIN_2 |
                          ADCx_CHANNEL_PIN_3 | ADCx_CHANNEL_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADCx_CHANNEL_GPIO_PORT, &GPIO_InitStruct);
}

/**
 * @brief De-Initializate ADC hardware resources
 * @param hadc: ADC handle pointer
 * @retval None
 */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc){
    /*##-1- Reset peripherals ################################################*/
    ADCx_FORCE_RESET();
    ADCx_RELEASE_RESET();

    /*##-2- Disable peripherals and GPIO Clocks ##############################*/
    HAL_GPIO_DeInit(ADCx_CHANNEL_GPIO_PORT, ADCx_CHANNEL_PIN_1);
    HAL_GPIO_DeInit(ADCx_CHANNEL_GPIO_PORT, ADCx_CHANNEL_PIN_2);
    HAL_GPIO_DeInit(ADCx_CHANNEL_GPIO_PORT, ADCx_CHANNEL_PIN_3);
    HAL_GPIO_DeInit(ADCx_CHANNEL_GPIO_PORT, ADCx_CHANNEL_PIN_4);
}
