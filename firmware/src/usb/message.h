/**
 * @file
 * @date	09.11.2015
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef USB_MESSAGE_H_
#define USB_MESSAGE_H_

#include "stdint.h"

#define USB_HEADER_SIZE			(8)
#define USB_MAX_PAYLOAD_LENGTH	(56)
#define USB_MAGIC_NUMBER		(43981)
#define USB_MAGIC_BYTE1			((unsigned char)0xAB)
#define USB_MAGIC_BYTE2			((unsigned char)0xCD)

#define USB_MAX_MESSAGE_LENGTH 	(USB_MAX_PAYLOAD_LENGTH + USB_HEADER_SIZE)


/**
 * @brief	Different types of data transferred over USB.
 */
typedef enum usb_packet_type {
	USB_PACKET  = 0,	///< SkyNet data packet
	USB_CONTROL = 1,	///< Node control message
	USB_INFO    = 2,	///< Pure information
	USB_DEBUG   = 3		///< Debugging string (as from sprintf)
} usb_packet_type;

/**
 * @brief	A "message" transferred over USB.
 *
 * Can be real networking packet, or control commands, debug strings, ...
 * The field \ref magic must be 43981 (0xAB 0xCD) and helps the other end
 * to determine the byte order.
 *
 * (Field seqno is especially important for correct alignment.)
 */
typedef struct usb_message {
	uint16_t			magic;			///< @brief Must be 43981.
	char				type;			///< @brief Should ne \ref usb_packet_type.
	char				seqno;			///< @brief	Increasing sequence number.
	uint32_t 			payload_length;	///< @brief Number of payload bytes.
	char*				payload;		///< @brief Actual payload bytes.
} usb_message;

#endif /* USB_MESSAGE_H_ */
