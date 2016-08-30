/**
 * @file
 * @brief SkyNet-feather basic config class.
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <Arduino.h>

class skynetfeatherconfig {
    public:
        bool compareMac(uint8_t m0, uint8_t m1);

        uint8_t radio_pa_level = 23;

        uint8_t mac0 = 0;
        uint8_t mac1 = 0;
        uint32_t serial0 = 0;
        uint32_t serial1 = 0;
        uint32_t serial2 = 0;
        uint32_t serial3 = 0;
};


#endif // CONFIG_H_
