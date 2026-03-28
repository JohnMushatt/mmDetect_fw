#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "mm_uart.h"
#include "mm_defines.h"
#include "ld2450.h"
#include "mm_wifi.h"
#include "mm_udp.h"
#include "led_strip.h"


static const char *TAG = "app_main";
void app_main(void)
{
    //gpio_reset_pin(BLINK_GPIO);
    //gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    int stat = OK;
    stat = mm_wifi_init();
    if( stat != ESP_OK)
    {
        ESP_LOGE(TAG, "WiFi initialization failed");
        return;
    }
    stat = mm_udp_init("192.168.1.100", 5000);
    if( stat != ESP_OK)
    {
        ESP_LOGE(TAG, "UDP initialization failed");
        return;
    }
    mm_udp_tx_queue = xQueueCreate(MM_UDP_QUEUE_LEN, MM_UDP_FRAME_SIZE);
    if (mm_udp_tx_queue == NULL) {
        ESP_LOGE(TAG, "Queue creation failed");
        return;
    }

    led_strip_handle_t led_strip;
    led_strip_config_t strip_config = {
    .strip_gpio_num = GPIO_NUM_38,
    .max_leds = 1,
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10 MHz
        .flags.with_dma = false,  // Try without DMA first

    };
    stat = led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip);
    if( stat != OK)
    {
        ESP_LOGE(TAG, "LED strip initialization failed");
        return;
    }
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "LED strip initialization successful");



    stat = OK;//mm_uart1_init();
    if( stat != OK)
    {
        ESP_LOGE(TAG, "UART driver installation failed");
        return;
    }
    ESP_LOGI(TAG, "UART1 initialization successful. Listening for LD2450 data");

    vTaskDelay(pdMS_TO_TICKS(1000));

    //ESP_LOGI(TAG, "UART2 initialization successful");
    //ESP_LOGI(TAG, "Starting LD2450 task");
    //xTaskCreate(ld2450_task, "ld2450_task", 4096, NULL, 10, NULL);

    xTaskCreate(mm_udp_tx_task, "mm_udp_tx_task", 4096, NULL, 5, NULL);
    ESP_LOGI(TAG, "udp tx task created");
    xTaskCreate(ld2450_sim_task, "ld2450_sim_task", 4096, NULL, 6, NULL);
    ESP_LOGI(TAG, "sim task created");
    while (1) {
        led_strip_set_pixel(led_strip, 0, 0, 16, 0);
        led_strip_refresh(led_strip);
        vTaskDelay(pdMS_TO_TICKS(1000));
        led_strip_clear(led_strip);
        vTaskDelay(pdMS_TO_TICKS(1000));
        //vTaskDelay(pdMS_TO_TICKS(1000));

    }
}