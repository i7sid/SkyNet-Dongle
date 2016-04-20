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


#ifdef __cplusplus
extern "C" {
#endif


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
        uint8_t dest_address[8];
        uint8_t src_pan_id[2];
        uint8_t src_address[8];
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

/**
 * @brief	Parses the content of a packet from a buffer into a \ref mac_frame_data structure.
 * @param	frame	Data structure to write data to.
 * @param	buffer	Data buffer from for example PHY to read data from.
 * @param	length	Total bytes received (important for correct detection of payload size)
 *
 * @return	Exact number of bytes read.
 */
uint16_t mac_frame_data_unpack(mac_frame_data *frame, uint8_t *buffer, uint16_t length);


#ifdef __cplusplus
}
#endif

#endif /* !MAC_FRAME_H */
