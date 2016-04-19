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

#include "../misc/misc.h"
#include "../cpu/systick.h"
#include <stdlib.h>
#include "mac_config.h"


/**
 * @brief	Tries to send raw bytes over PHY using CSMA/CA.
 *
 * @return	True, if sent successfully, false otherwise.
 */
bool mac_transmit_data(uint8_t* data, uint16_t length);


typedef enum mac_frame_types {
    MAC_FRAME_BEACON    = 0x0,
    MAC_FRAME_DATA      = 0x1,
    MAC_FRAME_ACK       = 0x2,
    MAC_FRAME_MAC_CMD   = 0x3
} mac_frame_types;

typedef enum mac_addr_modes {
    MAC_ADDR_MODE_NOT        = 0x0,
    MAC_ADDR_MODE_RESERVED   = 0x1,
    MAC_ADDR_MODE_SHORT      = 0x2,
    MAC_ADDR_MODE_LONG       = 0x3
} mac_addr_modes;


#define MHR_FC_GET_FRAME_TYPE(x) 			({ (x[0] >> 5) & 0x7;  })
#define MHR_FC_SET_FRAME_TYPE(x, n) 		({ x[0] = (x[0] & 0x1F) | ((n & 0x7) << 5); })
#define MHR_FC_GET_SECURITY_ENABLED(x) 		({ (x[0] >> 4) & 0x1;  })
#define MHR_FC_SET_SECURITY_ENABLED(x, n) 	({ x[0] = (x[0] & 0xEF) | ((n & 0x1) << 4); })
#define MHR_FC_GET_FRAME_PENDING(x) 		({ (x[0] >> 3) & 0x1;  })
#define MHR_FC_SET_FRAME_PENDING(x, n) 		({ x[0] = (x[0] & 0xF7) | ((n & 0x1) << 3); })
#define MHR_FC_GET_ACK_REQUEST(x) 			({ (x[0] >> 2) & 0x1;  })
#define MHR_FC_SET_ACK_REQUEST(x, n) 		({ x[0] = (x[0] & 0xFB) | ((n & 0x1) << 2); })
#define MHR_FC_GET_PAN_ID_COMPRESSION(x) 	({ (x[0] >> 1) & 0x1;  })
#define MHR_FC_SET_PAN_ID_COMPRESSION(x, n) ({ x[0] = (x[0] & 0xFD) | ((n & 0x1) << 1); })

#define MHR_FC_GET_DEST_ADDR_MODE(x) 		({ (x[1] >> 4) & 0x3;  })
#define MHR_FC_SET_DEST_ADDR_MODE(x, n) 	({ x[1] = (x[1] & 0xCF) | ((n & 0x3) << 4);  })
#define MHR_FC_GET_FRAME_VERSION(x) 		({ (x[1] >> 2) & 0x3;  })
#define MHR_FC_SET_FRAME_VERSION(x, n) 		({ x[1] = (x[1] & 0xF3) | ((n & 0x3) << 2);  })
#define MHR_FC_GET_SRC_ADDR_MODE(x) 		({ (x[1]) & 0x3;  })
#define MHR_FC_SET_SRC_ADDR_MODE(x, n) 		({ x[1] = (x[1] & 0xFC) | (n & 0x3);  })



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



#include "mac_frame_data.h"
#include "mac_frame_ack.h"
#include "util.h"

/**
 * @brief	Tries to send a packet over PHY using \ref max_transmit_data.
 *
 * @return	True, if sent successfully, false otherwise.
 */
bool mac_transmit_packet(mac_frame_data* frame);

/**
 * @brief	Rrturns a random integer strict smaller than max.
 */
int random(int max);

#endif /* !MAC_H */
