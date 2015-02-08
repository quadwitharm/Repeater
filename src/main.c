#include "stm32f4xx_hal.h"
#include "uart.h"
#include "joystick.h"
#include "task.h"

int main(void){
    HAL_Init();

    UART_init(9600);
    JOYSTICK_init();
    setTransfer();

    kputs(SEND_DIST_CONSOLE, "Uart Test\r\n");
    JoystickRead_Task();
    xTaskCreate(JoystickRead_Task,
            (portCHAR *)"Joystick Task",
            256,
            NULL,
            tskIDLE_PRIORITY + 1,
            NULL);

    vTaskStartScheduler();
}

