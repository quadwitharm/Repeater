#include "stm32f4xx_hal.h"
#include "uart.h"

int main(void){
    HAL_Init();

    UART_init(9600);
    setTransfer();
    while(1){}
}
