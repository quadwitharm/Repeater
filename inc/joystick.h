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

/* Definition for ADCx clock resources */
#define ADCx                            ADC3
#define ADCx_CLK_ENABLE()               __ADC3_CLK_ENABLE()
#define DMAx_CLK_ENABLE()               __DMA2_CLK_ENABLE()
#define ADCx_CHANNEL_GPIO_CLK_ENABLE()  __GPIOF_CLK_ENABLE()

#define ADCx_FORCE_RESET()              __ADC_FORCE_RESET()
#define ADCx_RELEASE_RESET()            __ADC_RELEASE_RESET()

/* Definition for ADCx Channel Pin */
#define ADCx_CHANNEL_PIN                GPIO_PIN_10
#define ADCx_CHANNEL_GPIO_PORT          GPIOF

/* Definition for ADCx's Channel */
#define ADCx_CHANNEL                    ADC_CHANNEL_8

/* Definition for ADCx's DMA */
#define ADCx_DMA_CHANNEL                DMA_CHANNEL_2
#define ADCx_DMA_STREAM                 DMA2_Stream0

/* Definition for ADCx's NVIC */
#define ADCx_DMA_IRQn                   DMA2_Stream0_IRQn
#define ADCx_DMA_IRQHandler             DMA2_Stream0_IRQHandler

#ifdef __cplusplus
}
#endif

#endif /* __JOYSTICK_H__ */
