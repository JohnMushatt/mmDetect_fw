#include "ld2450.h"
#include "freertos/projdefs.h"
#include "mm_uart.h"
#include <stdint.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "mm_udp.h"
#include <math.h>
#include <string.h>
#include "esp_timer.h"
#include "mm_protocol.h"
static const char *TAG = "ld2450";
int ld2450_init(void)
{
    int stat = OK;
    return stat;
}

/**
 * @brief Encode a sign magnitude value
 * @param val The value to encode
 * @param low The low byte of the value
 * @param high The high byte of the value
 */
static void encode_sign_magnitude(int16_t val, uint8_t *low, uint8_t *high)
{
    uint16_t raw = (val < 0) ? (0x8000 | (uint16_t)(-val)) : (uint16_t)val;
    *low  = raw & 0xFF;
    *high = (raw >> 8) & 0xFF;
}
/**
 * @brief Build a simulated LD2450 frame
 * @param buf The buffer to build the frame
 * @param frame The frame to build
 */
void ld2450_build_sim_frame(uint8_t *buf, const ld2450_frame_t *frame)
{
    memcpy(buf, FRAME_HEADER, 4);
    uint8_t *p = buf + 4;
    for (int i = 0; i < LD2450_MAX_TARGETS; i++) {
        const ld2450_target_t *t = &frame->targets[i];
        encode_sign_magnitude(t->x_mm,      &p[0], &p[1]);
        encode_sign_magnitude(t->y_mm,      &p[2], &p[3]);
        encode_sign_magnitude(t->speed_cms, &p[4], &p[5]);
        p[6] = t->resolution_mm & 0xFF;
        p[7] = (t->resolution_mm >> 8) & 0xFF;
        p += 8;
    }
    memcpy(buf + 28, FRAME_TAIL, 2);
}


/**
 * @brief Parse a LD2450 frame
 * @param buf The buffer to parse
 * @param frame The frame to parse
 * @return true if the frame is valid, false otherwise
 */
bool ld2450_parse_frame(const uint8_t *buf, ld2450_frame_t *frame)
{
    if (memcmp(buf, FRAME_HEADER, sizeof(FRAME_HEADER)) != 0)
        return false;
    if (memcmp(buf + LD2450_FRAME_SIZE - 2, FRAME_TAIL, sizeof(FRAME_TAIL)) != 0)
        return false;
    frame->target_count = 0;
    const uint8_t *p = buf + 4;
    for (int i = 0; i < LD2450_MAX_TARGETS; i++) {
        int16_t  x    =  decode_sign_magnitude(p[0], p[1]);//(int16_t)(p[0] | (p[1] << 8));
        int16_t  y    =  decode_sign_magnitude(p[2], p[3]);//(int16_t)(p[2] | (p[3] << 8));
        int16_t  spd  =  decode_sign_magnitude(p[4], p[5]);//(int16_t)(p[4] | (p[5] << 8));
        uint16_t res  = (uint16_t)(p[6] | (p[7] << 8));
        frame->targets[i].x_mm          = x;
        frame->targets[i].y_mm          = y;
        frame->targets[i].speed_cms     = spd;
        frame->targets[i].resolution_mm = res;
        if (x != 0 || y != 0 || spd != 0)
            frame->target_count++;
        p += 8;
    }
    return true;
}

/**
 * @brief Decode a sign magnitude value
 * @param low The low byte of the value
 * @param high The high byte of the value
 * @return The decoded value
 */
int16_t decode_sign_magnitude(uint8_t low, uint8_t high)
{
    uint16_t raw = low | (high << 8);
    int16_t magnitude = raw & 0x7FFF;
    if (raw & 0x8000)
    {
        magnitude = -magnitude;
    }
    return magnitude;
}

void ld2450_sim_task(void *pvParameter)
{
    uint8_t proto_buf[MM_PROTO_TARGET_FRAME_SIZE];
    uint32_t frame_counter = 0;
    float angle = 0.0f;
    while (1) {
        ld2450_frame_t frame = {0};
        frame.target_count = 1;
        frame.targets[0].x_mm          = (int16_t)(1000.0f * sinf(angle));
        frame.targets[0].y_mm          = (int16_t)(2000.0f + 500.0f * cosf(angle));
        frame.targets[0].speed_cms     = 50;
        frame.targets[0].resolution_mm = 10;
        angle += 0.05f;
        uint32_t ts = (uint32_t)(esp_timer_get_time() / 1000);
        mm_proto_build_target_frame(proto_buf, frame_counter++, ts, &frame);
        xQueueSend(mm_udp_tx_queue, proto_buf, portMAX_DELAY);
        ESP_LOGI(TAG, "Sent frame %lu", (unsigned long)frame_counter);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
extern uint8_t mm_uart1_rx_buffer[MM_UART_BUF_SIZE_256];
/**
 * @brief Task to receive and parse LD2450 frames from UART
 * @param pvParameter The parameter for the task
 */
void ld2450_task(void *pvParameter)
{
    int frame_idx = 0, stat = OK;
    //ld2450_frame_t frame;
    uint8_t rx_buf[MM_UART_BUF_SIZE_512];
    uint8_t frame_buf[LD2450_FRAME_SIZE];
    //char out[256];
    int count = 0;
    while(1)
    {
        int len = uart_read_bytes(MM_UART_1, rx_buf, MM_UART_BUF_SIZE_512, pdMS_TO_TICKS(10));
        if( len > 0)
        {
            //ESP_LOGI(TAG, "Received %d bytes", len);
        }
        else
        {
            count++;
            if(count > 40)
            {
                ESP_LOGI(TAG, "No data received");
                count = 0;
            }
            continue;
        }
        // Loop through the received data and build the frame buffer
        for(int i = 0; i < len; i++)
        {
            frame_buf[frame_idx] = rx_buf[i];
            frame_idx++;
            if( frame_idx < 4)
            {
                if(frame_buf[frame_idx -1] != FRAME_HEADER[frame_idx -1])
                {
                    frame_idx = 0;
                }
                continue;
            }

            if(frame_idx == LD2450_FRAME_SIZE)
            {
                // Parse the frame
                ld2450_frame_t frame;
                
                bool validFrame = ld2450_parse_frame(frame_buf, &frame);

                if(validFrame)
                {
                    //ESP_LOGI(TAG, "Valid Frame");

                    uint8_t proto_buf[MM_PROTO_TARGET_FRAME_SIZE];
                    static uint32_t frame_counter = 0;
                    uint32_t ts = (uint32_t)(esp_timer_get_time() / 1000);
                    mm_proto_build_target_frame(proto_buf, frame_counter++, ts, &frame);
                    xQueueSend(mm_udp_tx_queue, proto_buf, portMAX_DELAY);
                    //vTaskDelay(pdMS_TO_TICKS(10));


                }
                //ESP_LOGI(TAG, "Frame received");
                frame_idx = 0;
            }
        }

    }
}


