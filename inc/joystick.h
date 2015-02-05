#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f429i_discovery.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include "clib.h"

extern ADC_HandleTypeDef    AdcHandle;
bool JOYSTICK_init();
void JoystickReadTask();

/* Definition for ADCx clock resources */
#define ADCx                            ADC3
#define ADCx_CLK_ENABLE()               __ADC3_CLK_ENABLE()
#define DMAx_CLK_ENABLE()               __DMA2_CLK_ENABLE()
#define ADCx_CHANNEL_GPIO_CLK_ENABLE()  __GPIOF_CLK_ENABLE()

#define ADCx_FORCE_RESET()              __ADC_FORCE_RESET()
#define ADCx_RELEASE_RESET()            __ADC_RELEASE_RESET()

/* Definition for ADCx Channel Pin */
#define ADCx_CHANNEL_PIN_1              GPIO_PIN_7
#define ADCx_CHANNEL_PIN_2              GPIO_PIN_8
#define ADCx_CHANNEL_PIN_3              GPIO_PIN_9
#define ADCx_CHANNEL_PIN_4              GPIO_PIN_10
#define ADCx_CHANNEL_GPIO_PORT          GPIOF

/* Definition for ADCx's Channel */
#define ADCx_CHANNEL_1                  ADC_CHANNEL_5
#define ADCx_CHANNEL_2                  ADC_CHANNEL_6
#define ADCx_CHANNEL_3                  ADC_CHANNEL_7
#define ADCx_CHANNEL_4                  ADC_CHANNEL_8

#define ADCx_CHANNEL_COUNT              4

#ifdef __cplusplus
}
#endif

#endif /* __JOYSTICK_H__ */
