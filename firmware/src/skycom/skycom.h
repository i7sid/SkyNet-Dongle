/**
 *  @file
 *
 *  @date 	10.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */

#ifndef SKYCOM_SKYCOM_H_
#define SKYCOM_SKYCOM_H_

#include "chip.h"
#include "../mac/mac.h"
#include "../cpu/nv_storage.h"
#include "../cpu/v_storage.h"
#include "../misc/event_queue.h"


/**
 * @brief	Builds and sends an dongle comand answer.
 */
void skycom_send_donglecmd_answer(uint8_t dest[], uint8_t seq_no, uint8_t reply);


/**
 * @brief	Generates regularily events to send wind data.
 */
void generate_event_wind_base(void);

/**
 * @brief	Generates regularily events to send position data.
 */
void generate_event_pos_base(void);



#endif /* SKYCOM_SKYCOM_H_ */
