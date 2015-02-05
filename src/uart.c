#include <stdbool.h>
#include "uart.h"
#include "clib.h"
#include "task.h"

UART_HandleTypeDef UartHandle1;
UART_HandleTypeDef UartHandle2;

uint8_t buffer1[64]; /* A size enough to all command */
uint8_t buffer2[64]; /* A size enough to all command */
uint8_t *pbuf1 = buffer1, *pbuf2 = buffer2;

void UART_init( uint32_t BaudRate){
    UartHandle1 = (UART_HandleTypeDef) {
        .Instance = USART1,
        .Init = { .BaudRate = BaudRate,
            .WordLength = UART_WORDLENGTH_8B,
            .StopBits = UART_STOPBITS_1,
            .Parity = UART_PARITY_NONE,
            .HwFlowCtl = UART_HWCONTROL_NONE,
            .Mode = UART_MODE_TX_RX
        }
    };
    UartHandle2 = (UART_HandleTypeDef) {
        .Instance = USART3,
        .Init = { .BaudRate = BaudRate,
            .WordLength = UART_WORDLENGTH_8B,
            .StopBits = UART_STOPBITS_1,
            .Parity = UART_PARITY_NONE,
            .HwFlowCtl = UART_HWCONTROL_NONE,
            .Mode = UART_MODE_TX_RX
        }
    };
    HAL_UART_Init(&UartHandle1);
    HAL_UART_Init(&UartHandle2);
}

void UART_send(uint8_t *buf,int len){
    while(len--)
        HAL_UART_Transmit(&UartHandle1, buf++, 1, 10000);
}

void setTransfer(){
    HAL_UART_Receive_IT(&UartHandle2, pbuf1++, 1);
    HAL_UART_Receive_IT(&UartHandle1, pbuf2++, 1);
}

/**
 * @brief  Rx Transfer completed callback
 * @param  UartHandle: UART handle
 * @retval None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle){
    if(UartHandle->Instance == USART1){
        HAL_UART_Receive_IT(&UartHandle1, pbuf1++, 1); /* Listen to next byte */
        if(*(pbuf1-1) == 0x86){
            taskENTER_CRITICAL();
            HAL_UART_Transmit_IT(&UartHandle2,buffer1, pbuf1 - buffer1); /* Forward to UART3 */
            taskEXIT_CRITICAL();
        }
    }else if(UartHandle->Instance == USART3){
        HAL_UART_Receive_IT(&UartHandle2, pbuf2++, 1); /* Listen to next byte */
        if(*(pbuf1-1) == 0x86){
            taskENTER_CRITICAL();
            HAL_UART_Transmit_IT(&UartHandle1,buffer2, pbuf2 - buffer2); /* Forward to UART1 */
            taskEXIT_CRITICAL();
        }
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle){
    /* Ignore error and just forward the data in DR,
     * Error will detected by checksum */
    uint8_t tmp = UartHandle->Instance->DR;
    /* Same as RxCpltCallBack */
    if(UartHandle->Instance == USART1){
        *pbuf1 = tmp;
    }else if(UartHandle->Instance == USART3){
        *pbuf2 = tmp;
    }
    /* Listen to next byte, if a command is fully received,
     * forward to another port */
    HAL_UART_RxCpltCallback(UartHandle);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart){
    GPIO_InitTypeDef GPIO_InitStruct = {
        .Mode = GPIO_MODE_AF_PP,
        .Pull = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FAST,
    };
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /*##-2- Configure peripheral GPIO ##########################################*/
    /*##-3- Configure the NVIC for UART ########################################*/
    if(huart->Instance == USART1){// tx/rx: PA9/PA10, PB6/PB7
        __USART1_CLK_ENABLE();
        __GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(USART1_IRQn, 12, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }else if(huart->Instance == USART2){// tx/rx: PA2/PA3, PD5/PD6
        __USART2_CLK_ENABLE();
        __GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(USART2_IRQn, 12, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
    }else if(huart->Instance == USART3){// tx/rx: PB10/PB11, PC10/PC11, PD8/PD9
        __USART3_CLK_ENABLE();
        __GPIOB_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
        GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(USART3_IRQn, 12, 0);
        HAL_NVIC_EnableIRQ(USART3_IRQn);
    }else if(huart->Instance == UART4){// tx/rx: PA0/PA1, PC10/PC11
        __UART4_CLK_ENABLE();
        __GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
        GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(UART4_IRQn, 12, 0);
        HAL_NVIC_EnableIRQ(UART4_IRQn);
    }else if(huart->Instance == UART5){// tx/rx: PC12/PD2
        __UART5_CLK_ENABLE();
        __GPIOC_CLK_ENABLE();
        __GPIOD_CLK_ENABLE();
        GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
        GPIO_InitStruct.Pin = GPIO_PIN_12;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        GPIO_InitStruct.Pin = GPIO_PIN_2;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(UART5_IRQn, 12, 0);
        HAL_NVIC_EnableIRQ(UART5_IRQn);
    }else if(huart->Instance == USART6){// tx/rx: PC6/PC7, PG14/PG9
        __USART6_CLK_ENABLE();
        __GPIOC_CLK_ENABLE();
        GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
        GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(USART6_IRQn, 12, 0);
        HAL_NVIC_EnableIRQ(USART6_IRQn);
    }else if(huart->Instance == UART7){// tx/rx: PE8/PE7, PF7/PF6
        __UART7_CLK_ENABLE();
        __GPIOE_CLK_ENABLE();
        GPIO_InitStruct.Alternate = GPIO_AF8_UART7;
        GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_7;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(UART7_IRQn, 12, 0);
        HAL_NVIC_EnableIRQ(UART7_IRQn);
    }else if(huart->Instance == UART8){// tx/rx: PE1/PE0
        __UART8_CLK_ENABLE();
        __GPIOE_CLK_ENABLE();
        GPIO_InitStruct.Alternate = GPIO_AF8_UART8;
        GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_0;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(UART8_IRQn, 12, 0);
        HAL_NVIC_EnableIRQ(UART8_IRQn);
    }else return;
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *huart){
    /*##-1- Reset peripherals ##################################################*/
    /*##-2- Disable peripherals and GPIO Clocks #################################*/
    /*##-3- Disable the NVIC for UART ##########################################*/
    if(huart->Instance == USART1){// tx/rx: PA9/PA10, PB6/PB7
        __USART1_FORCE_RESET();
        __USART1_RELEASE_RESET();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);
        HAL_NVIC_DisableIRQ(USART1_IRQn);
    }else if(huart->Instance == USART2){// tx/rx: PA2/PA3, PD5/PD6
        __USART2_FORCE_RESET();
        __USART2_RELEASE_RESET();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_3);
        HAL_NVIC_DisableIRQ(USART2_IRQn);
    }else if(huart->Instance == USART3){// tx/rx: PB10/PB11, PC10/PC11, PD8/PD9
        __USART3_FORCE_RESET();
        __USART3_RELEASE_RESET();
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10 | GPIO_PIN_11);
        HAL_NVIC_DisableIRQ(USART3_IRQn);
    }else if(huart->Instance == UART4){// tx/rx: PA0/PA1, PC10/PC11
        __UART4_FORCE_RESET();
        __UART4_RELEASE_RESET();
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0 | GPIO_PIN_1);
        HAL_NVIC_DisableIRQ(UART4_IRQn);
    }else if(huart->Instance == UART5){// tx/rx: PC12/PD2
        __UART5_FORCE_RESET();
        __UART5_RELEASE_RESET();
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_12);
        HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);
        HAL_NVIC_DisableIRQ(UART5_IRQn);
    }else if(huart->Instance == USART6){// tx/rx: PC6/PC7, PG14/PG9
        __USART6_FORCE_RESET();
        __USART6_RELEASE_RESET();
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6 | GPIO_PIN_7);
        HAL_NVIC_DisableIRQ(USART6_IRQn);
    }else if(huart->Instance == UART7){// tx/rx: PE8/PE7, PF7/PF6
        __UART7_FORCE_RESET();
        __UART7_RELEASE_RESET();
        HAL_GPIO_DeInit(GPIOE, GPIO_PIN_0 | GPIO_PIN_1);
        HAL_NVIC_DisableIRQ(UART7_IRQn);
    }else if(huart->Instance == UART8){// tx/rx: PE1/PE0
        __UART8_FORCE_RESET();
        __UART8_RELEASE_RESET();
        HAL_GPIO_DeInit(GPIOE, GPIO_PIN_0 | GPIO_PIN_1);
        HAL_NVIC_DisableIRQ(UART8_IRQn);
    }else return;
}

/**
 * @brief  These function handles UART interrupt request.
 * @param  None
 * @retval None
 * @Note   This function is redefined in "main.h" and related to DMA stream
 *         used for USART data transmission
 */
void USART1_IRQHandler(void){
    HAL_UART_IRQHandler(&UartHandle1);
}
void USART2_IRQHandler(void){
}
void USART3_IRQHandler(void){
    HAL_UART_IRQHandler(&UartHandle2);
}
void UART4_IRQHandler(void){
}
void UART5_IRQHandler(void){
}
void USART6_IRQHandler(void){
}
void UART7_IRQHandler(void){
}
void UART8_IRQHandler(void){
}
