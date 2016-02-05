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

#ifndef MAC_FRAME_DATA_H
#define MAC_FRAME_DATA_H

#include <string.h>
#include "mac_config.h"
#include "util.h"



typedef struct mac_frame_data {
    struct {
        uint8_t frame_control[2];
        uint8_t seq_no;
        uint8_t address[MAC_CONFIG_ADDRESS_LENGTH];
        // uint8_t aux_security_header[0];
    } __attribute__((packed)) mhr;
    uint8_t* payload;
    uint8_t fcs[2];
    uint16_t payload_size;
} __attribute__((packed)) mac_frame_data;


static inline size_t mac_frame_data_get_size(mac_frame_data *frame) {
    return sizeof(frame->mhr) + frame->payload_size + sizeof(frame->fcs);
}

static inline void mac_frame_data_pack(mac_frame_data *frame, uint8_t *buffer) {
    // copy MHR
    memcpy(buffer, (uint8_t*)(&(frame->mhr)), sizeof(frame->mhr));

    // copy payload
    memcpy(buffer + sizeof(frame->mhr), frame->payload, frame->payload_size);

    // copy fcs
    memcpy(buffer + sizeof(frame->mhr) + frame->payload_size, frame->fcs,
            sizeof(frame->fcs));
}

#endif /* !MAC_FRAME_H */
