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

#include "../../firmware/src/mac/mac_frame_data.h"
#include "../../firmware/src/mac/mac_frame_ack.h"


int main(int argc, char** argv) {
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
