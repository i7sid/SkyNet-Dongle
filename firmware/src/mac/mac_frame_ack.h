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
} __attribute__((aligned(1),packed)) mac_frame_ack;

inline void mac_frame_ack_init(mac_frame_ack *frame) {
	memset(frame, 0, sizeof(mac_frame_ack));
	MHR_FC_SET_FRAME_TYPE(frame->frame_control, MAC_FRAME_ACK);
	MHR_FC_SET_FRAME_VERSION(frame->frame_control, 0x01);
}

#endif /* !MAC_FRAME_ACK_H */
