/**
 * @file
 *
 * @brief   TODO
 *
 * TODO
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-02-05
 */

#ifndef MAC_H
#define MAC_H

#include <inttypes.h>
#include "mac_config.h"


typedef enum mac_frame_types {
    MAC_FRAME_BEACON    = 0x0,
    MAC_FRAME_DATA      = 0x1,
    MAC_FRAME_ACK       = 0x2,
    MAC_FRAME_MAC_CMD   = 0x3
} mac_frame_types;


#define MHR_FC_GET_FRAME_TYPE(x) { (x[0] >> 5) & 0x7  }
#define MHR_FC_SET_FRAME_TYPE(x, n) { x[0] = (x[0] & 0xF8) | (n & 0x7) }

#define MHR_FC_GET_SECURITY_ENABLED(x) { (x[0] >> 4) & 0x1  }
#define MHR_FC_GET_FRAME_PENDING(x) { (x[0] >> 3) & 0x1  }
#define MHR_FC_GET_ACK_REQUEST(x) { (x[0] >> 2) & 0x1  }
#define MHR_FC_GET_PAN_ID_COMPRESSION(x) { (x[0] >> 1) & 0x1  }


/*
typedef struct mac_frame_control {
    uint8_t     frame_type          : 3;
    uint8_t     security_enabled    : 1;
    uint8_t     frame_pending       : 1;
    uint8_t     ack_request         : 1;
    uint8_t     pan_id_compression  : 1;
    uint8_t     reserved            : 3;
    uint8_t     dest_addr_mode      : 2;
    uint8_t     frame_version       : 2;
    uint8_t     source_addr_mode    : 2;

} __attribute__((packed))  mac_frame_control;
*/


#endif /* !MAC_H */
