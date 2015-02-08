#ifndef __UART_H__
#define __UART_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f429i_discovery.h"
#include "stm32f4xx_hal.h"

void UART_init(uint32_t BaudRate);
void UART_send(int to, uint8_t *buf,int len);
void UART_recv(uint8_t *,int); // not implemented yet
void setTransfer();
void USARTx_IRQHandler(void);

#define SEND_DIST_CONSOLE 1
#define SEND_DIST_FLIGHT 2

#ifdef __cplusplus
}
#endif

#endif /* __UART_H__ */
