/**
 * @file
 *
 * @brief   TODO
 *
 * TODO
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-02-02
 */

#ifndef MAC_FRAME_ACK_H
#define MAC_FRAME_ACK_H

#include "mac_config.h"
#include "util.h"

#define mac_frame_ack_calc_crc(frame) { \
        mac_frame_calc_crc((uint8_t*)frame, sizeof(mac_frame_ack)); }

typedef struct mac_frame_ack {
    uint8_t frame_control[2];
    uint8_t seq_no;
    uint8_t fcs[2];
} __attribute__((packed)) mac_frame_ack;


#endif /* !MAC_FRAME_ACK_H */
