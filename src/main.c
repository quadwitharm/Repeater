#include "stm32f4xx_hal.h"
#include "uart.h"
#include "joystick.h"
#include "send.h"
#include "arm_math.h"

#define X_MID_VALUE 1291
#define Y_MID_VALUE 2037
#define ADC_MAX_VALUE 4095
#define ADC_MIN_VALUE 0
#define ROLL_MAX 20
#define PITCH_MAX 20
#define CENTER_AREA 0.03

int main(void){
    HAL_Init();

    UART_init(9600);
    JOYSTICK_init();
    setTransfer();

    kputs("Uart Test\r\n");

    while(1){
        uint16_t X = 0, Y = 0;
        float x = 1.0, y = 10;
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

        x = (X > X_MID_VALUE) ?
            (float)(X - X_MID_VALUE) / (ADC_MAX_VALUE - X_MID_VALUE) :
            (float)(X - X_MID_VALUE) / (X_MID_VALUE - ADC_MIN_VALUE);

        y = (Y > Y_MID_VALUE) ?
            (float)(Y - Y_MID_VALUE) / (ADC_MAX_VALUE - Y_MID_VALUE) :
            (float)(Y - Y_MID_VALUE) / (Y_MID_VALUE - ADC_MIN_VALUE);

        x = ((x > 0 ? x : -x) < CENTER_AREA) ? 0 : x;
        y = ((y > 0 ? y : -y) < CENTER_AREA) ? 0 : y;

        // Temporary use gyro plot canvas to see the output
        float g[3] = { 0, x * ROLL_MAX, y * PITCH_MAX};

        SendCommand_3( 0x01 , 0x00, (uint8_t *)g, 12);
    }
}
