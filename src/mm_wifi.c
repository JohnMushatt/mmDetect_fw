#include "mm_wifi.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

static const char *TAG = "mm_wifi";

static EventGroupHandle_t wifi_event_group;

#define WIFI_CONNECTED_BIT BIT0

esp_err_t mm_wifi_init(void)
{
    esp_err_t stat = ESP_OK;
}