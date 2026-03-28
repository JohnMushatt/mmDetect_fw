#ifndef LD2450_H
#define LD2450_H

#include "driver/uart.h"
#include "mm_defines.h"
#include "esp_log.h"


#define LD2450_FRAME_SIZE 30
#define LD2450_MAX_TARGETS 3
// LD2450 frame structure
static const uint8_t FRAME_HEADER[] = {0xAA, 0xFF, 0x03, 0x00};
static const uint8_t FRAME_TAIL[]   = {0x55, 0xCC};
typedef struct {
    int16_t  x_mm;
    int16_t  y_mm;
    int16_t  speed_cms;
    uint16_t resolution_mm;
} ld2450_target_t;
typedef struct {
    ld2450_target_t targets[LD2450_MAX_TARGETS];
    int             target_count;
} ld2450_frame_t;

int ld2450_init(void);
int16_t decode_sign_magnitude(uint8_t low, uint8_t high);

void ld2450_task(void *pvParameter);
void ld2450_sim_task(void *pvParameter);
void ld2450_build_sim_frame(uint8_t *buf, const ld2450_frame_t *frame);
bool ld2450_parse_frame(const uint8_t *buf, ld2450_frame_t *frame);
static void encode_sign_magnitude(int16_t val, uint8_t *low, uint8_t *high);
#endif // LD2450_H