/**
 * @file
 *
 * @date		25.10.2015
 * @author		Michael Zapf <michael.zapf@fau.de>
 */

#ifndef USB_SKYNET_CDC_H_
#define USB_SKYNET_CDC_H_


#include "chip.h"

#include <stdio.h>
#include <string.h>

#include "app_usbd_cfg.h"
#include "cdc_vcom.h"
#include "stopwatch.h"
#include "../misc/event_queue.h"



/**
 * @brief	Initializes the USB driver as a USB CDC virtual com port device (client).
 *
 * This also calls \ref skynet_cdc_connect() after a successful setup procedure.
 *
 * @return	\b 0 in case of an error, any other number means \b success.
 */
int skynet_cdc_init(void);


/**
 * @brief	Initializes connection to USB host.
 *
 * This has fairly the same effect as plugging the cable in.
 * This function is called by \ref skynet_cdc_init() automatically.
 *
 * After this function was called the host can start the enumeration process.
 *
 * @note	This function does not block until host accepts connection.
 */
void skynet_cdc_connect(void);

/**
 * @brief	Shuts the connection to USB host down.
 *
 * This has fairly the same effect as plugging the cable out.
 */
void skynet_cdc_disconnect(void);


/**
 * @brief	Checks if a usb connection is established.
 *
 * @return	\b true (not zero) if a connection is establish, \b false (0) otherwise.
 */
bool skynet_cdc_connected(void);

/**
 * @brief	Reads non-blocking all available characters into pBuf.
 * @return	Number of read bytes.
 */
uint32_t skynet_cdc_read(uint8_t *pBuf, uint32_t buf_len);


/**
 * @brief	Sends len characters from pBuf to usb host.
 * @return	Number of written bytes.
 */
uint32_t skynet_cdc_write(uint8_t *pBuf, uint32_t len);



#endif /* USB_SKYNET_CDC_H_ */
