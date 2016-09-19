/**
 * @file
 *
 * @date	10.05.2016
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief
 */

#ifndef CPU_V_STORAGE_H_
#define CPU_V_STORAGE_H_

#include "chip.h"

typedef struct V_DATA_T {
	uint32_t	time_wind_wait;
	uint32_t	time_pos_wait;
} __attribute__((aligned(1),packed)) V_DATA_T;


void skynet_v_init(void);


extern V_DATA_T v_configuration;


#endif /* CPU_V_STORAGE_H_ */
