// mm_udp.c
#include "mm_udp.h"
#include "lwip/sockets.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "mm_udp";
static int sock = -1;
static struct sockaddr_in dest_addr;
QueueHandle_t mm_udp_tx_queue = NULL;

/**
 * @brief Initialize the UDP socket
 * @param dest_ip The IP address of the destination
 * @param dest_port The port of the destination
 * @return OK if successful, ERROR otherwise
 */
int mm_udp_init(const char *dest_ip, uint16_t dest_port)
{
    int stat = ESP_OK;
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if( sock  < 0)
    {
        ESP_LOGE(TAG, "Failed to create socket: errno %d", errno);
        return ESP_FAIL;
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(dest_port);
 
    if( inet_pton(AF_INET, dest_ip, &dest_addr.sin_addr) <= 0)
    {
        ESP_LOGE(TAG, "Invalid IP address: %s", dest_ip);
        close(sock);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "UDP Info: %s:%d", dest_ip, dest_port);
    return stat;
}
/**
 * @brief Task to send data to the destination over UDP
 * @param pvParameter The parameter for the task
 */
void mm_udp_tx_task(void *pvParameter)
{
    uint8_t frame[MM_UDP_FRAME_SIZE];
    while (1) {
        if (xQueueReceive(mm_udp_tx_queue, frame, portMAX_DELAY) == pdTRUE) {
            mm_udp_send(frame, MM_UDP_FRAME_SIZE);
        }
    }
}

/**
 * @brief Send data to the destination over UDP
 * @param data The data to send
 * @param len The length of the data
 * @return The number of bytes sent if successful, ERROR otherwise
 */
int mm_udp_send(const uint8_t *data, size_t len)
{
    int sent = sendto(sock, data, len, 0,
                      (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (sent < 0) {
        ESP_LOGE(TAG, "sendto() failed: errno %d", errno);
    }
    return sent;
}