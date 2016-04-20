/**
 * @file
 * @brief   TODO
 *
 * TODO
 *
 * @author  Michael Zapf <michael.zapf@fau.de>
 * @date    2016-02-04
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "../../firmware/src/mac/mac.h"
#include "../../firmware/src/mac/mac_frame_data.h"
#include "../../firmware/src/mac/mac_frame_ack.h"

int main(int argc, char** argv) {
    mac_frame_data *frame = mac_frame_data_calloc();

    MHR_FC_SET_FRAME_TYPE(frame->mhr.frame_control, MAC_FRAME_DATA);
    MHR_FC_SET_PAN_ID_COMPRESSION(frame->mhr.frame_control, 0);
    MHR_FC_SET_DEST_ADDR_MODE(frame->mhr.frame_control, MAC_ADDR_MODE_LONG);
    MHR_FC_SET_SRC_ADDR_MODE(frame->mhr.frame_control, MAC_ADDR_MODE_SHORT);

    frame->mhr.src_pan_id[0] = 0xAA;
    frame->mhr.src_pan_id[1] = 0xAA;
    frame->mhr.dest_pan_id[0] = 0xAA;
    frame->mhr.dest_pan_id[1] = 0xAA;

    printf("0x%x\n", (uint8_t)(MHR_FC_GET_SRC_ADDR_MODE(frame->mhr.frame_control)));
    printf("0x%x\n", (uint8_t)(MHR_FC_GET_DEST_ADDR_MODE(frame->mhr.frame_control)));
    frame->payload = calloc(7, sizeof(uint8_t));
    memcpy(frame->payload, "Hallo!", 7);
    frame->payload_size = (uint16_t)strlen((char*)frame->payload) + 1;

    mac_transmit_packet(frame);

/*
    mac_frame_data frame;
    memset(&frame, 0, sizeof(mac_frame_data));
    frame.payload = (uint8_t*)"Hallo!";
    frame.payload_size = (uint16_t)strlen((char*)frame.payload) + 1;

    frame.mhr.frame_control[0]  = 0x1;
    frame.mhr.frame_control[1]  = 0x2;
    frame.mhr.seq_no            = 0x3;
    frame.mhr.address[0]        = 0x4;
    frame.mhr.address[1]        = 0x5;
    frame.mhr.address[2]        = 0x6;
    frame.mhr.address[3]        = 0x7;
*/
/*
    size_t size = mac_frame_data_get_size(&frame);
    printf("%i %lu\n", frame.payload_size, size);

    for (int i = 0; i < size; i++) {
        printf("0x%x  ", ((uint8_t*)&frame)[i]);
    }
    printf("\n");


    uint8_t buffer[size];
    memset(&buffer, 0, sizeof(buffer));
    mac_frame_data_pack(&frame, buffer);

    mac_frame_calc_crc(buffer, size);
    printf("crc: 0x%x 0x%x\n", frame.fcs[0], frame.fcs[1]);

    for (int i = 0; i < size; i++) {
        printf("0x%x  ", buffer[i]);
    }
    printf("\n");
*/

//    mac_transmit_packet(&frame);

    /*


    printf("0x%x  0x%x  0x%x  0x%x  0x%x  0x%x  0x%x  ",
            frame.frame_control[0], frame.frame_control[1], frame.seq_no,
            frame.address[0], frame.address[1], frame.address[2], frame.address[3]);
    for (size_t i = 0; i < strlen((char*)frame.payload)+1; i++) {
        printf("0x%x  ", ((uint8_t*)frame.payload)[i]);
    }
    printf("0x%x  0x%x",
            frame.fcs[0], frame.fcs[1]);
    printf("\n");

    for (int i = 0; i < 7 + strlen((char*)frame.payload) + 1 + 2; i++) {
        printf("0x%x  ", buffer[i]);
    }
    printf("\n");

    return(0);


    mac_frame_ack ack;
    memset(&ack, 0, sizeof(mac_frame_ack));
    mac_frame_ack_calc_crc(&ack);
    printf("crc: 0x%x 0x%x\n", ack.fcs[0], ack.fcs[1]);

*/

    return EXIT_SUCCESS;
}


void si446x_get_modem_status(uint8_t i) {
    // TODO stub
}
void msDelay(uint32_t i) {
    // TODO stub
}
void radio_send_variable_packet(uint8_t* buf, uint16_t length) {
    printf("######################################################################\n");
    printf("Sending packet of length: %d\n\n", length);
    printf("Bytes:");

    for (int i = 0; i < length; ++i) {
        if (!(i % 8)) {
            printf("\n%d\t", i);
        }
        printf("0x%02x ", buf[i]);
    }

    printf("\n######################################################################\n");
}




struct si446x_reply_GET_MODEM_STATUS_map {
    uint8_t  MODEM_PEND;
    uint8_t  MODEM_STATUS;
    uint8_t  CURR_RSSI;
    uint8_t  LATCH_RSSI;
    uint8_t  ANT1_RSSI;
    uint8_t  ANT2_RSSI;
    uint16_t AFC_FREQ_OFFSET;
};



/* The union that stores the reply written back to the host registers */
union si446x_cmd_reply_union {
    uint8_t RAW[16];
    struct  si446x_reply_GET_MODEM_STATUS_map  GET_MODEM_STATUS;
};

union si446x_cmd_reply_union Si446xCmd;
