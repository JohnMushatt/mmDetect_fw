#ifndef MM_FRAME_H
#define MM_FRAME_H

#include "ld2450.h"
#include <stdint.h>
#include <string.h>

#define MM_FRAME_SIZE 38

size_t ld2450_serialize_frame(const ld2450_frame_t *frame, uint32_t frame_id,
                              uint32_t timestamp_ms, uint8_t *buf);

#endif // MM_FRAME_H