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


// TODO: Verallgemeinern, damit nur eine mac_transmit_*-Funktion benötigt wird?
// 		 ("Superklasse"/"Elternklasse")


typedef struct mac_frame_data {
    struct {
        uint8_t frame_control[2];
        uint8_t seq_no;
        uint8_t dest_pan_id[2];
        uint8_t address[MAC_MAX_ADDRESS_LENGTH];
        uint8_t src_pan_id[2];
        // uint8_t aux_security_header[0];			// for future usage?
    } __attribute__((packed)) mhr;
    uint8_t* payload;
    uint8_t fcs[2];
    uint16_t payload_size;
} __attribute__((packed)) mac_frame_data;


/**
 * @brief	Allocates the memory for a frame and initializes memory.
 */
void* mac_frame_data_calloc(void);

/**
 * @brief	Frees the memory used by a frame (including payload).
 */
void mac_frame_data_free(mac_frame_data *frame);


//size_t mac_frame_data_get_size(mac_frame_data *frame);

/**
 * @brief	Estimates the maximum size of the size needed by a frame.
 *
 * Can be used to allocate a raw buffer used to pack frame into.
 */
size_t mac_frame_data_estimate_size(mac_frame_data *frame);

void mac_frame_data_init(mac_frame_data *frame);


/**
 * @brief	Writes packet into buffer that can be passed to phy_transmit.
 * @param	frame	Pointer to frame in RAM that should be sent.
 * @param	buffer	Pointer to buffer in RAM, must be allocated large enough!
 *
 * @return	Exact number of bytes written.
 */
uint16_t mac_frame_data_pack(mac_frame_data *frame, uint8_t *buffer);


#endif /* !MAC_FRAME_H */
