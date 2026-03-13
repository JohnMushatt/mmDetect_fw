#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "mm_uart.h"
#include "mm_defines.h"
#include "ld2450.h"
#define BLINK_GPIO GPIO_NUM_13
static const char *TAG = "app_main";
void app_main(void)
{
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);



    int stat = mm_uart1_init();
    if( stat != OK)
    {
        ESP_LOGE(TAG, "UART driver installation failed");
        return;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "UART1 initialization successful");
    ESP_LOGI(TAG, "Starting LD2450 task");
    xTaskCreate(ld2450_task, "ld2450_task", 4096, NULL, 10, NULL);
    ESP_LOGI(TAG, "LD2450 task started");
    while (1) {
        gpio_set_level(BLINK_GPIO, 1);
        ESP_LOGI(TAG, "LED ON");
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(BLINK_GPIO, 0);
        ESP_LOGI(TAG, "LED OFF");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}