#include "mm_protocol.h"
#include <string.h>

uint8_t mm_proto_checksum(const uint8_t *data, size_t len)
{
    uint8_t cs = 0;
    for (size_t i = 0; i < len; i++)
        cs ^= data[i];
    return cs;
}

static inline void put_u16_le(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)(v);
    p[1] = (uint8_t)(v >> 8);
}

static inline void put_u32_le(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v);
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

static inline void put_i16_le(uint8_t *p, int16_t v)
{
    put_u16_le(p, (uint16_t)v);
}

size_t mm_proto_build_target_frame(
    uint8_t *buf,
    uint32_t frame_id,
    uint32_t timestamp_ms,
    const ld2450_frame_t *targets)
{
    size_t pos = 0;

    buf[pos++] = MM_PROTO_HEADER_0;
    buf[pos++] = MM_PROTO_HEADER_1;

    buf[pos++] = MM_MSG_TYPE_TARGET;

    put_u16_le(&buf[pos], MM_PROTO_TARGET_PAYLOAD_LEN);
    pos += 2;

    put_u32_le(&buf[pos], frame_id);
    pos += 4;

    put_u32_le(&buf[pos], timestamp_ms);
    pos += 4;

    buf[pos++] = (uint8_t)targets->target_count;

    for (int i = 0; i < LD2450_MAX_TARGETS; i++) {
        if (i < targets->target_count) {
            const ld2450_target_t *t = &targets->targets[i];
            put_i16_le(&buf[pos],     t->x_mm);
            put_i16_le(&buf[pos + 2], t->y_mm);
            put_i16_le(&buf[pos + 4], t->speed_cms);
            put_u16_le(&buf[pos + 6], t->resolution_mm);
        } else {
            memset(&buf[pos], 0, 8);
        }
        pos += 8;
    }

    /* XOR bytes [2 .. pos-1] (msg_type through end of target data) */
    buf[pos] = mm_proto_checksum(&buf[2], pos - 2);
    pos++;

    buf[pos++] = MM_PROTO_TAIL_0;
    buf[pos++] = MM_PROTO_TAIL_1;

    return pos;
}