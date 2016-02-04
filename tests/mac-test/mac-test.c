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

unsigned short crc16(const unsigned char* data_p, unsigned char length) {
    unsigned char x;
    unsigned short crc = 0xFFFF;

    while (length--) {
        x = crc >> 8 ^ *data_p++;
        x ^= x >> 4;
        crc =   (crc << 8) ^
                ((unsigned short)(x << 12)) ^
                ((unsigned short)(x << 5)) ^
                ((unsigned short)x);
    }
    return crc;
}

void mac_frame_data_calc_crc(mac_frame_data* frame) {
    printf("sizeof: %lu\n", sizeof(mac_frame_data));
    unsigned short crc = crc16((const unsigned char*)frame, sizeof(mac_frame_data) - 2);
    printf("crc: %u\n", crc);
}

int main(int argc, char** argv) {
    mac_frame_data frame;
    memset(&frame, 0, sizeof(mac_frame_data));

    mac_frame_data_calc_crc(&frame);

    printf("Hallo\n");
    return EXIT_SUCCESS;
}
