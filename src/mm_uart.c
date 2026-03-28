#include "mm_uart.h"
#include "driver/uart.h"
#include "mm_defines.h"
#include "esp_log.h"
#include "soc/clk_tree_defs.h"
#define TAG "mm_uart"
uint8_t mm_uart1_rx_buffer[MM_UART_BUF_SIZE_256];
uint8_t mm_uart1_tx_buffer[MM_UART_BUF_SIZE_256];
int mm_uart1_init(void)
{
    int stat = OK;
    uart_config_t mm_uart1_config = {
        .baud_rate = MM_UART_BAUD_256000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT
    };

    // Install the UART driver and get the handle
    stat = uart_driver_install(MM_UART_1,
         MM_UART_BUF_SIZE_1024, 
         MM_UART_BUF_SIZE_1024, 
         0, 
         NULL, 
         0);
    if( stat != OK)
    {
        ESP_LOGE(TAG, "UART1 driver installation failed");
        return ERROR;
    }
    
    stat = uart_param_config(MM_UART_1, &mm_uart1_config);
    if( stat != OK)
    {
        ESP_LOGE(TAG, "UART0 parameter configuration failed");
        return ERROR;
    }

    stat = uart_set_pin(MM_UART_1, MM_UART_1_TX_PIN, MM_UART_1_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if( stat != OK)
    {
        ESP_LOGE(TAG, "UART0 pin configuration failed");
        return ERROR;
    }
    return stat;
}
int mm_uart2_init(void)
{
    int stat = OK;
    uart_config_t mm_uart2_config = {
        .baud_rate = MM_UART_BAUD_115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT
    };

    // Install the UART driver and get the handle
    stat = uart_driver_install(MM_UART_2, MM_UART_BUF_SIZE_256, MM_UART_BUF_SIZE_256, 0, NULL, 0);
    if( stat != OK)
    {
        ESP_LOGE(TAG, "UART driver installation failed");
        return ERROR;
    }
    
    stat = uart_param_config(MM_UART_2, &mm_uart2_config);
    if( stat != OK)
    {
        ESP_LOGE(TAG, "UART2 parameter configuration failed");
        return ERROR;
    }
    stat = uart_set_pin(MM_UART_2, MM_UART_2_TX_PIN, MM_UART_2_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if( stat != OK)
    {
        ESP_LOGE(TAG, "UART2 pin configuration failed");
        return ERROR;
    }
    return stat;
}
