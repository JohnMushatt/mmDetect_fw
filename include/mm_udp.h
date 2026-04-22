#ifndef MM_UDP_H
#define MM_UDP_H
#include <stdint.h>
#include <stddef.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "mm_protocol.h"
#define MM_UDP_QUEUE_LEN  20
#define MM_UDP_FRAME_SIZE MM_PROTO_TARGET_FRAME_SIZE

extern QueueHandle_t mm_udp_tx_queue;

int mm_udp_init(const char *dest_ip, uint16_t dest_port);
int mm_udp_send(const uint8_t *data, size_t len);
void mm_udp_tx_task(void *pvParameter);
#endif