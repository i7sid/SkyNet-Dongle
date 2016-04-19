/**
 * @file
 *
 * @date	15.04.2016
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief
 */

#ifndef CPU_NV_STORAGE_H_
#define CPU_NV_STORAGE_H_

#include "chip.h"

#define SECTOR_STARTADDR 	(0x0000F000)
#define SECTOR_NUM       	(15)
#define SECTOR_SIZE 		(0x1000)

#define SECTOR_EMPTY        (0xFF)
#define SECTOR_VALID        (0xAA)


typedef struct NV_DATA_T {
	char	sector_state;
	char	mac_addr[8];
	char	ipv4_addr[4];
	int32_t	compass_x_offset;
	int32_t	compass_y_offset;
	int32_t	compass_z_offset;
	char	reserved_padding[231];
} __attribute__((packed)) NV_DATA_T;


void skynet_nv_init(void);

NV_DATA_T* skynet_nv_get(void);

bool skynet_nv_write(NV_DATA_T* new_data);



#endif /* CPU_NV_STORAGE_H_ */
