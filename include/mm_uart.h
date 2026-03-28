/**
 * @file mm_uart.h
 * @brief UART communication for mmDetect device
 */ 

 #ifndef MM_UART_H
 #define MM_UART_H

#include "driver/uart.h"
#include "hal/uart_types.h"
#include "soc/clk_tree_defs.h"
#include "driver/gpio.h"



// UART 1
#define MM_UART_1     UART_NUM_1
#define MM_UART_1_TX_PIN   GPIO_NUM_16
#define MM_UART_1_RX_PIN   GPIO_NUM_17
#define MM_UART_BAUD_256000     256000
#define MM_UART_BUF_SIZE_256       256
#define MM_UART_BUF_SIZE_512       512
int mm_uart1_init(void);
void mm_uart1_task(void *pvParameter);

// UART 2
#define MM_UART_2           UART_NUM_2
#define MM_UART_2_TX_PIN   GPIO_NUM_43
#define MM_UART_2_RX_PIN   GPIO_NUM_44
#define MM_UART_BAUD_115200     115200
#define MM_UART_BUF_SIZE_1024       1024
int mm_uart2_init(void);
void mm_uart2_task(void *pvParameter);
#endif // MM_UART_H