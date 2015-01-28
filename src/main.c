#include "stm32f4xx_hal.h"
#include "uart.h"
#include "joystick.h"

uint16_t X = 0;

int main(void){
    HAL_Init();

    UART_init(9600);
    JOYSTICK_init();
    setTransfer();

    kputs("Uart Test\r\n");

    HAL_ADC_Start(&AdcHandle);
    while(1){
        HAL_ADC_PollForConversion(&AdcHandle, 10000);
        if(HAL_ADC_GetState(&AdcHandle) == HAL_ADC_STATE_EOC_REG)
        {
            X = HAL_ADC_GetValue(&AdcHandle);
        }
        kprintf("%d\r\n", X);
    }
}
