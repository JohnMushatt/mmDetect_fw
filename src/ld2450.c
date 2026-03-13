#include "ld2450.h"
#include "freertos/projdefs.h"
#include "mm_uart.h"
static const char *TAG = "ld2450";
int ld2450_init(void)
{
    int stat = OK;
    return stat;
}


static bool ld2450_parse_frame(const uint8_t *buf, ld2450_frame_t *frame)
{
    if (memcmp(buf, FRAME_HEADER, sizeof(FRAME_HEADER)) != 0)
        return false;
    if (memcmp(buf + LD2450_FRAME_SIZE - 2, FRAME_TAIL, sizeof(FRAME_TAIL)) != 0)
        return false;
    frame->target_count = 0;
    const uint8_t *p = buf + 4;
    for (int i = 0; i < LD2450_MAX_TARGETS; i++) {
        int16_t  x    = (int16_t)(p[0] | (p[1] << 8));
        int16_t  y    = (int16_t)(p[2] | (p[3] << 8));
        int16_t  spd  = (int16_t)(p[4] | (p[5] << 8));
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



extern uint8_t mm_uart1_rx_buffer[MM_UART_BUF_SIZE_256];
void ld2450_task(void *pvParameter)
{
    int frame_idx = 0;
    //ld2450_frame_t frame;
    uint8_t rx_buf[MM_UART_BUF_SIZE_256];
    uint8_t frame_buf[LD2450_FRAME_SIZE];
    while(1)
    {
        int len = uart_read_bytes(MM_UART_1, rx_buf, MM_UART_BUF_SIZE_256, pdMS_TO_TICKS(100));
        if( len > 0)
        {
            ESP_LOGI(TAG, "Received %d bytes", len);
        }
        else
        {
            ESP_LOGI(TAG, "No data received");
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
                ESP_LOGI(TAG, "Frame received");
                frame_idx = 0;
            }
        }

        //vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


