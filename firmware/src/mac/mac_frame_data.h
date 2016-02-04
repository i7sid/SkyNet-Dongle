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

#include "mac_config.h"


typedef struct mac_frame_data {
    uint8_t frame_control[2];
    uint8_t seq_no;
    uint8_t address[MAC_CONFIG_ADDRESS_LENGTH];
//    uint8_t aux_security_header[0];
//    uint8_t *payload;
    uint8_t fcs[2];
} mac_frame_data;
//} __attribute__((packed)) mac_frame_data;


#endif /* !MAC_FRAME_H */
