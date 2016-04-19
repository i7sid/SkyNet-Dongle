/**
 * @file
 *
 * @date	15.04.2016
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief
 */


#include "nv_storage.h"
#include "systick.h"
#include <string.h>


static NV_DATA_T read __attribute__ ((aligned (8)));



static inline void skynet_nv_read(NV_DATA_T* dst) {
	NV_DATA_T* src = (NV_DATA_T*)(SECTOR_STARTADDR);
	// prepare
	//Chip_IAP_PreSectorForReadWrite(SECTOR_NUM, SECTOR_NUM);
	//msDelayActive(1);
	memcpy(dst, src, sizeof(NV_DATA_T));
}

void skynet_nv_init(void) {
	// read from flash sector
	skynet_nv_read(&read);
}

NV_DATA_T* skynet_nv_get(void) {
	return &read;
}

bool skynet_nv_write(NV_DATA_T* new_data) {
	__disable_irq();
	uint8_t r = 0;

	// prepare
	r += Chip_IAP_PreSectorForReadWrite(SECTOR_NUM, SECTOR_NUM);

	// erase
	r += Chip_IAP_EraseSector(SECTOR_NUM, SECTOR_NUM);

	// prepare again
	r += Chip_IAP_PreSectorForReadWrite(SECTOR_NUM, SECTOR_NUM);

	// write
	r += Chip_IAP_CopyRamToFlash(SECTOR_STARTADDR, (uint32_t *)new_data, sizeof(NV_DATA_T));

	// read again, just for sure
	skynet_nv_read(&read);

	__enable_irq();
	return (r == 0);
}
