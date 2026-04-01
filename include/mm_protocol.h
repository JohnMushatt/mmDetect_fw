#ifndef MM_PROTOCOL_H
#define MM_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>
#include "ld2450.h"

#define MM_PROTO_HEADER_0           0xBB
#define MM_PROTO_HEADER_1           0xBB
#define MM_PROTO_TAIL_0             0xEE
#define MM_PROTO_TAIL_1             0xEE

#define MM_MSG_TYPE_TARGET          0x01
#define MM_MSG_TYPE_IQ              0x02

/* header(2) + msg_type(1) + payload_len(2) + checksum(1) + tail(2) */
#define MM_PROTO_OVERHEAD           8

/* frame_id(4) + timestamp_ms(4) + target_count(1) + 3×target(8) */
#define MM_PROTO_TARGET_PAYLOAD_LEN 33
#define MM_PROTO_TARGET_FRAME_SIZE  (MM_PROTO_OVERHEAD + MM_PROTO_TARGET_PAYLOAD_LEN) /* 41 */

uint8_t mm_proto_checksum(const uint8_t *data, size_t len);

size_t mm_proto_build_target_frame(
    uint8_t *buf,
    uint32_t frame_id,
    uint32_t timestamp_ms,
    const ld2450_frame_t *targets
);

#endif