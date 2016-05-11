/**
 *  @file
 *
 *  @date 	10.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef SKYCOM_RF_PKT_HANDLER_H_
#define SKYCOM_RF_PKT_HANDLER_H_

#include "chip.h"

#include <stdlib.h>
#include <string.h>

#include "skycom.h"
#include "../cpu/systick.h"
#include "../mac/mac.h"
#include "../radio/skynet_radio.h"
#include "../usb/skynet_cdc.h"
#include "../periph/led.h"
#include "../cpu/nv_storage.h"
#include "../cpu/v_storage.h"


/**
 * @brief	Processes an incoming skynet packet.
 */
void skynet_received_packet(skynet_packet *pkt);


#endif /* SKYCOM_RF_PKT_HANDLER_H_ */
