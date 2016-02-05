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

    mac_frame_data_calc_crc(&frame);
    printf("crc: 0x%x 0x%x\n", frame.fcs[0], frame.fcs[1]);

    mac_frame_ack ack;
    memset(&ack, 0, sizeof(mac_frame_ack));
    mac_frame_ack_calc_crc(&ack);
    printf("crc: 0x%x 0x%x\n", ack.fcs[0], ack.fcs[1]);


    return EXIT_SUCCESS;
}
