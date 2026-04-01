// in ld2450.c — add:
#include <string.h>
#include "mm_frame.h"
#include <stdint.h>
#include "ld2450.h"
static uint8_t xor_checksum(const uint8_t *data, size_t len)
{
    uint8_t cs = 0;
    for (size_t i = 0; i < len; i++)
        cs ^= data[i];
    return cs;
}

size_t ld2450_serialize_frame(const ld2450_frame_t *frame, uint32_t frame_id,
                              uint32_t timestamp_ms, uint8_t *buf)
{
    size_t off = 0;

    buf[off++] = 0xBB;
    buf[off++] = 0xBB;

    memcpy(&buf[off], &frame_id, 4);      off += 4;
    memcpy(&buf[off], &timestamp_ms, 4);   off += 4;
    buf[off++] = (uint8_t)frame->target_count;

    for (int i = 0; i < LD2450_MAX_TARGETS; i++) {
        int16_t  x   = frame->targets[i].x_mm;
        int16_t  y   = frame->targets[i].y_mm;
        int16_t  spd = frame->targets[i].speed_cms;
        uint16_t res = frame->targets[i].resolution_mm;
        memcpy(&buf[off], &x,   2); off += 2;
        memcpy(&buf[off], &y,   2); off += 2;
        memcpy(&buf[off], &spd, 2); off += 2;
        memcpy(&buf[off], &res, 2); off += 2;
    }

    buf[off] = xor_checksum(&buf[2], off - 2);
    off++;

    buf[off++] = 0xEE;
    buf[off++] = 0xEE;

    return off; // always 38
}