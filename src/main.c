#include "stm32f4xx_hal.h"
#include "uart.h"
#include "joystick.h"

int main(void){
    HAL_Init();

    UART_init(9600);
    JOYSTICK_init();
    setTransfer();

    kputs("Uart Test\r\n");

    JoystickReadTask();
}
