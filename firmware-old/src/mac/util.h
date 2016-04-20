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

#ifndef UTIL_H
#define UTIL_H



/**
 * @Source http://stackoverflow.com/questions/10564491/function-to-calculate-a-crc16-checksum/23726131#23726131
 */
static inline uint16_t crc16(const uint8_t* data_p, uint8_t length) {
    uint8_t x;
    uint16_t crc = 0xFFFF;

    while (length--) {
        x = crc >> 8 ^ *data_p++;
        x ^= x >> 4;
        crc =   (crc << 8) ^
                ((uint16_t)(x << 12)) ^
                ((uint16_t)(x << 5)) ^
                ((uint16_t)x);
    }
    return crc;
}

static inline void mac_frame_calc_crc(uint8_t* frame, size_t frame_size) {
    uint16_t crc = crc16((const uint8_t*)frame, frame_size - 2);
    frame[frame_size-2] = (crc >> 8) & 0xFF;
    frame[frame_size-1] = (crc     ) & 0xFF;
}



#endif /* !UTIL_H */
