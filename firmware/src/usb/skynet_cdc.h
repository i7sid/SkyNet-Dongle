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
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "app_usbd_cfg.h"
#include "cdc_vcom.h"
#include "message.h"
#include "stopwatch.h"
#include "../misc/event_queue.h"


/**
 * @brief	Types for FSM controlling USB CDC RX.
 */
typedef enum usb_receive_state {
	USB_RECEIVE_IDLE,
	USB_RECEIVE_MAGIC,
	USB_RECEIVE_TYPE,
	USB_RECEIVE_SEQNO,
	USB_RECEIVE_PAYLOAD_LENGTH,
	USB_RECEIVE_PAYLOAD
} usb_receive_state;



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

/**
 * @brief	Writes len characters from pBuf into sending buffer.
 *
 * Bytes won't be sent until buffer is full or \ref skynet_cdc_flush() is called.
 *
 * @return	Number of written bytes.
 */
uint32_t skynet_cdc_write_buffered(uint8_t *pBuf, uint32_t len);

/**
 * @brief	Send the bytes in the buffer immediately.
 * @return	Number of written bytes.
 */
uint32_t skynet_cdc_flush(void);


/**
 * @brief	Sends a \ref usb_message to usb host.
 * @return	Number of written bytes.
 */
uint32_t skynet_cdc_write_message(usb_message *msg);

/**
 * @brief	Works like to printf and generates debug output message.
 *
 * Internally it uses vsnprintf to stay in maximum message size.
 */
uint32_t skynet_cdc_write_debug(const char* format, ... );

/**
 * @brief	Must be called if usb characters are available.
 *
 * Then they are processed.
 */
void skynet_cdc_receive_data(void);

/**
 * @brief	Task that should be called with 1kHz
 */
void skynet_cdc_task(void);

/**
 * @brief	 Deletes the contents of the TX buffer.
 */
void skynet_cdc_flush_buffers(void);


#endif /* USB_SKYNET_CDC_H_ */
