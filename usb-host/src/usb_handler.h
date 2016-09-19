/**
 *  @file
 *
 *  @date 	01.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef USB_HANDLER_H_
#define USB_HANDLER_H_

#include <usb/message.h>
#include <mac/mac.h>

void usbReceiveHandler(usb_message pkt);

#endif /* USB_HANDLER_H_ */
