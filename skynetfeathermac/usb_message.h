/**
 * @file
 * @date  09.11.2015
 * @author  Michael Zapf <michael.zapf@fau.de>
 */

#ifndef USB_MESSAGE_H_
#define USB_MESSAGE_H_

#include "stdint.h"

#define USB_HEADER_SIZE     (8)
//#define USB_MAX_PAYLOAD_LENGTH  (56)
//#define USB_MAX_PAYLOAD_LENGTH  (503)
#define USB_MAX_PAYLOAD_LENGTH  (1600)
#define USB_MAX_PACKET_SIZE   (64)
#define USB_MAGIC_NUMBER    (43981)
#define USB_MAGIC_BYTE1     ((unsigned char)0xAB)
#define USB_MAGIC_BYTE2     ((unsigned char)0xCD)

#define USB_MAX_MESSAGE_LENGTH  (USB_MAX_PAYLOAD_LENGTH + USB_HEADER_SIZE)


/**
 * @brief Different types of data transferred over USB.
 */
typedef enum usb_packet_type {
  USB_SKYNET_PACKET  = 0, ///< SkyNet data packet
  USB_CONTROL = 1,    ///< Node control message
  USB_INFO    = 2,    ///< Pure information
  USB_DEBUG   = 3,    ///< Debugging string (as from sprintf)
  USB_RANGE_TEST = 4    ///< Range test message with local GPS
} usb_packet_type;

/**
 * @brief Different types of control messages.
 */
typedef enum usb_ctrl_msg_type {
  USB_CTRL_RESET      = 0, ///< Reset the node
  USB_CTRL_BOOTLOADER   = 1, ///< Reset the node and switch to bootloader
  USB_CTRL_CALIB_COMPASS  = 2, ///< Start calibration sequence of compass
  USB_CTRL_SET_MAC_ADDR   = 3, ///< Set the MAC address of the node
  USB_CTRL_SET_IP_ADDR    = 4, ///< Set the IPv4 address of the node
  USB_CTRL_GET_MAC_ADDR   = 5, ///< Get the MAC address of the node via debug
  USB_CTRL_GET_IP_ADDR    = 6  ///< Get the IPv4 address of the node via debug
} usb_ctrl_msg_type;

/**
 * @brief A "message" transferred over USB.
 *
 * Can be real networking packet, or control commands, debug strings, ...
 * The field \ref magic must be 43981 (0xAB 0xCD) and helps the other end
 * to determine the byte order.
 *
 * (Field seqno is especially important for correct alignment.)
 */
typedef struct usb_message {
  uint16_t      magic;      ///< @brief Must be 43981.
  char        type;     ///< @brief Should ne \ref usb_packet_type.
  char        seqno;      ///< @brief Increasing sequence number.
  uint32_t      payload_length; ///< @brief Number of payload bytes.
  char*       payload;    ///< @brief Actual payload bytes.
} usb_message;

#endif /* USB_MESSAGE_H_ */

