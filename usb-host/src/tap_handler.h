/**
 *  @file
 *
 *  @date 	01.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef NO_TAP

#ifndef TAP_HANDLER_H_
#define TAP_HANDLER_H_

#include <usb/message.h>
#include <mac/mac.h>

void tapReceiveHandler(void *pkt, size_t nread);


#endif /* TAP_HANDLER_H_ */

#endif // NO_TAP
