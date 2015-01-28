#include "stm32f4xx_hal.h"
#include "uart.h"
#include "joystick.h"


int main(void){
    HAL_Init();

    UART_init(9600);
    JOYSTICK_init();
    setTransfer();

    kputs("Uart Test\r\n");

    while(1){
        uint16_t X = 0, Y = 0;
        ADC_SwitchChannel(0);
        HAL_ADC_PollForConversion(&AdcHandle, 10000);
        if(HAL_ADC_GetState(&AdcHandle) == HAL_ADC_STATE_EOC_REG)
        {
            X = HAL_ADC_GetValue(&AdcHandle);
        }

        ADC_SwitchChannel(1);
        HAL_ADC_PollForConversion(&AdcHandle, 10000);
        if(HAL_ADC_GetState(&AdcHandle) == HAL_ADC_STATE_EOC_REG)
        {
            Y = HAL_ADC_GetValue(&AdcHandle);
        }

        kprintf("%d %d\r\n", X, Y);
    }
}
