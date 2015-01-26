#include "stm32f4xx_hal.h"
#include "uart.h"
#include "joystick.h"

__IO uint16_t uhADCxConvertedValue = 0;

int main(void){
    HAL_Init();

    UART_init(9600);
    JOYSTICK_init();
    setTransfer();

    while(1){
        HAL_ADC_Start_DMA(&AdcHandle, (uint32_t*)&uhADCxConvertedValue, 1);
    }
}
