/**
 * @file
 *
 * @date	01.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 *
 * @brief	Contains functionality for communication with RF module.
 */

#include <string.h>
#include <stdlib.h>

#include "skynet_radio.h"
#include "radio_config.h"
#include "radio_hal.h"
#include "../spi/spi.h"
#include "../cpu/systick.h"
#include "../misc/event_queue.h"
#include "../periph/led.h"
#include "../gpio/gpio_irq.h"

uint8_t bMain_IT_Status;
uint8_t pwrLvlIdx = 0;
uint8_t pwrLvl[] = {8,12,19,35,127};	//0, 5, 10, 15, 20 dBm
char timeStr[] = {0, 0, 0, 0, 0, 0};

volatile bool radio_initialized = false;

void radio_pin_init(void) {
	// "on/off" pin
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, RADIO_ON_PORT, RADIO_ON_PIN);
	Chip_GPIO_SetPinState(LPC_GPIO, RADIO_ON_PORT, RADIO_ON_PIN, true);
	//Chip_GPIO_SetPinState(LPC_GPIO, RADIO_ON_PORT, RADIO_ON_PIN, false);
}

void radio_init(void) {
	radio_pin_init();
	Chip_GPIOINT_Init(LPC_GPIOINT);

	// "on/off" pin
	Chip_GPIO_SetPinState(LPC_GPIO, RADIO_ON_PORT, RADIO_ON_PIN, false);

	vRadio_Init();	// intialize radio chip

	//uint8_t ret = si446x_apply_patch();
	//DBG("patched: %d\n", ret);

	si446x_part_info();
	si446x_func_info();

	DBG("--- radio chip version information following ---\n");
	DBG("CHIPREV:   0x%x\n", Si446xCmd.PART_INFO.CHIPREV);
	DBG("CUSTOMER:  0x%x\n", Si446xCmd.PART_INFO.CUSTOMER);
	DBG("ID:        0x%x\n", Si446xCmd.PART_INFO.ID);
	DBG("PART:      0x%x\n", Si446xCmd.PART_INFO.PART);
	DBG("PBUILD:    0x%x\n", Si446xCmd.PART_INFO.PBUILD);
	DBG("ROMID:     0x%x\n", Si446xCmd.PART_INFO.ROMID);

	DBG("FUNC:      0x%x\n", Si446xCmd.FUNC_INFO.FUNC);
	DBG("PATCH:     0x%x\n", Si446xCmd.FUNC_INFO.PATCH);
	DBG("REVBRANCH: 0x%x\n", Si446xCmd.FUNC_INFO.REVBRANCH);
	DBG("REVEXT:    0x%x\n", Si446xCmd.FUNC_INFO.REVEXT);
	DBG("REVINT:    0x%x\n", Si446xCmd.FUNC_INFO.REVINT);
	DBG("------ end radio chip version information ------\n");

	// This interrupt should have highest priority to assure that we do not miss packets
	NVIC_SetPriority(EINT3_IRQn, 0);
	radio_set_irq();

	// start receiving...
	vRadio_StartRXlong(pRadioConfiguration->Radio_ChannelNumber);
	DBG("Radio RX started.\n");
}

void radio_shutdown(void) {
	radio_unset_irq();
	radio_hal_AssertShutdown();
	SPI_Deinit();

	Chip_GPIO_SetPinState(LPC_GPIO, RADIO_ON_PORT, RADIO_ON_PIN, true);
	//Chip_GPIO_SetPinState(LPC_GPIO, RADIO_ON_PORT, RADIO_ON_PIN, false);
}

void radio_set_irq(void) {
	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIOINT_PORT0, (1 << 19));
	Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIOINT_PORT0, (1 << 19));
	NVIC_ClearPendingIRQ(EINT3_IRQn);
	NVIC_EnableIRQ(EINT3_IRQn);


}

void radio_unset_irq(void) {
	NVIC_ClearPendingIRQ(EINT3_IRQn);
	NVIC_DisableIRQ(EINT3_IRQn);
}

void radio_disable_irq(void){
	disable_gpio_irq(0,19);
}

void radio_enable_irq(void){
	enable_gpio_irq(0,19);
}

void radio_reset_packet_size(void) {
	// Field 2 length
	Si446xCmd.GET_PROPERTY.DATA0 = 0x1F;
	Si446xCmd.GET_PROPERTY.DATA1 = 0xFF;
	si446x_set_property_lpc(0x12, 2, 0x11);
}

void radio_send_variable_packet(uint8_t *packet, uint16_t length)
{
	uint8_t data[length+2];
#ifdef DEBUG
	uint8_t status_ph[0xfff];
	uint8_t status_mod[0xfff];
	uint8_t status_chip[0xfff];
	uint16_t i = 0;
	memset(status_ph, 0, 0xfff);
	memset(status_mod, 0, 0xfff);
	memset(status_chip, 0, 0xfff);
#endif

	radio_disable_irq();

	// Leave RX state
	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_READY);

	// Read ITs, clear pending ones
	si446x_get_int_status(0u, 0u, 0u);

	// Reset the Tx Fifo
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_TX_BIT);

	// copy payload data to send buffer
	memcpy(data+2, packet, length);

	// Field 1 length
	Si446xCmd.GET_PROPERTY.DATA0 = 0x0;
	Si446xCmd.GET_PROPERTY.DATA1 = 0x2;
	si446x_set_property_lpc(0x12, 2, 0x0D);

	// Field 2 length
	Si446xCmd.GET_PROPERTY.DATA0 = length >> 8;
	Si446xCmd.GET_PROPERTY.DATA1 = length & 0xFF;
	si446x_set_property_lpc(0x12, 2, 0x11);

	data[0] = length >> 8;
	data[1] = length & 0xFF;

	int16_t remaining = length+2;
	uint8_t* ptr = data;
	volatile bool first_run = true;
	while(remaining > 0) {
		uint8_t nowLength;
		if (first_run) {
			nowLength = RADIO_MAX_PACKET_LENGTH;
		}
		else if (RADIO_TX_ALMOST_EMPTY_THRESHOLD < remaining) {
			nowLength = RADIO_TX_ALMOST_EMPTY_THRESHOLD;
		}
		else {
			nowLength = remaining;
		}

		// Fill the TX fifo with datas
		si446x_write_tx_fifo(nowLength, ptr);
		ptr += nowLength;
		remaining -= nowLength;

		if (first_run) {
			si446x_start_tx(pRadioConfiguration->Radio_ChannelNumber, 0x80, 0x0);
			first_run = false;
		}

		while (true) {
			si446x_get_int_status(0u, 0u, 0u);

#ifdef DEBUG
			// DEBUG
			if (Si446xCmd.GET_INT_STATUS.PH_PEND > 0) {
				status_ph[i] = Si446xCmd.GET_INT_STATUS.PH_PEND;
				status_mod[i] = Si446xCmd.GET_INT_STATUS.MODEM_STATUS;
				status_chip[i] = Si446xCmd.GET_INT_STATUS.CHIP_STATUS;
				++i;
			}
#endif

			if (Si446xCmd.GET_INT_STATUS.CHIP_STATUS & SI446X_CMD_GET_INT_STATUS_REP_FIFO_UNDERFLOW_OVERFLOW_ERROR_BIT ||
					Si446xCmd.GET_INT_STATUS.CHIP_STATUS & SI446X_CMD_GET_INT_STATUS_REP_CMD_ERROR_BIT) {

				DBG("[ERROR] RF chip reported error while sending.\n");
				//skynet_led_blink_red_passive(1000);

				// reset chip to assure correct behaviour next time
				radio_shutdown();
				msDelayActive(50);
				msDelay(100);
				radio_init(); // also reenables interrupts
				radio_reset_packet_size(); // reset size of Field 2
				return;
			}
			else if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_SENT_BIT) {
				if (remaining > 0) {
					// ERROR CASE! PACKET_SENT interrupt occurred,
					// even if not all remaining bytes have been put to FIFO
					DBG("[ERROR] PACKET_SENT, but remaining: %d\n", remaining);
					//skynet_led_blink_red_passive(1000);

					// clear remaining bytes to avoid pushing further bytes to FIFO
					remaining = 0;
				}

				break;
			}
			else if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_TX_FIFO_ALMOST_EMPTY_BIT) {
				// not all bytes sent yet, but chip is ready to receive more
				if (remaining > 0) break;
			}
		}


	}
	DBG("remaining: %d\n", remaining);

#ifdef DEBUG
	for (int j = 0; j < i; ++j) {
		DBG("status[%d] = %d %d %d\n", j, status_ph[j], status_mod[j], status_chip[j]);
	}
#endif

	radio_reset_packet_size(); // reset size of Field 2
	radio_enable_irq();
}



void radio_packet_handler(void) {
#ifdef DEBUG
	uint8_t status_ph[0xfff];
	uint8_t status_mod[0xfff];
	uint8_t status_chip[0xfff];
	uint16_t i = 0;
	memset(status_ph, 0, 0xfff);
	memset(status_mod, 0, 0xfff);
	memset(status_chip, 0, 0xfff);
#endif

	si446x_get_int_status(0u, 0u, 0u);

#ifdef DEBUG
	status_ph[i] = Si446xCmd.GET_INT_STATUS.PH_PEND;
	status_mod[i] = Si446xCmd.GET_INT_STATUS.MODEM_STATUS;
	status_chip[i] = Si446xCmd.GET_INT_STATUS.CHIP_STATUS;
	++i;
#endif

	radio_disable_irq();


	// error occurred
	if (Si446xCmd.GET_INT_STATUS.CHIP_STATUS & SI446X_CMD_GET_INT_STATUS_REP_FIFO_UNDERFLOW_OVERFLOW_ERROR_BIT ||
			Si446xCmd.GET_INT_STATUS.CHIP_STATUS & SI446X_CMD_GET_INT_STATUS_REP_CMD_ERROR_BIT) {

		DBG("[ERROR] RF chip reported error by interrupt: %d.\n", Si446xCmd.GET_INT_STATUS.CHIP_STATUS);
		skynet_led_blink_passive(500);

		// reset chip to assure correct behaviour next time
		events_enqueue(EVENT_RADIO_RESTART, NULL);
		return;
	}
	// Packet beginning or completely received
	else if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_RX_FIFO_ALMOST_FULL_BIT ||
			Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_BIT) {
		uint16_t length_low;
		uint16_t length_high;
		uint16_t length;

		si446x_get_packet_info(0x0, 0x0, 0x0);
		length_low = Si446xCmd.PACKET_INFO.LENGTH_7_0;
		length_high = (Si446xCmd.PACKET_INFO.LENGTH_15_8 & 0xFF);
		length = (length_high << 8) + length_low;

		uint8_t data[length+1];
		data[length] = 0;		// terminate with null character
		uint8_t* ptr = data;

		uint16_t remaining = length;
		while (remaining > 0) {
			uint8_t nowLength;
			if (remaining > 0xFF || RADIO_RX_ALMOST_FULL_THRESHOLD < remaining) {
				nowLength = RADIO_RX_ALMOST_FULL_THRESHOLD;
			}
			else {
				nowLength = remaining;
			}

			// read current data from RX FIFO
			si446x_read_rx_fifo(nowLength, ptr);
			ptr += nowLength;
			remaining -= nowLength;

			while (true) {
				si446x_get_int_status(0u, 0u, 0u);
#ifdef DEBUG
				if (Si446xCmd.GET_INT_STATUS.PH_PEND > 0) {
					++i;
					status_ph[i] = Si446xCmd.GET_INT_STATUS.PH_PEND;
					status_mod[i] = Si446xCmd.GET_INT_STATUS.MODEM_STATUS;
					status_chip[i] = Si446xCmd.GET_INT_STATUS.CHIP_STATUS;
				}
#endif
				if (Si446xCmd.GET_INT_STATUS.PH_STATUS & SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_BIT) {
					//DBG("RECEIVED, remaining: %d\n", remaining);
					break;
				}
				else if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_RX_FIFO_ALMOST_FULL_BIT) {
					break;
				}

			}
		}

		ptr = data;
		DBG("RX str (%d/%d)  : %s\n", remaining, length, ptr);

		// copy packet
		int copy_length = length;
		if (SKYNET_RADIO_MAX_SIZE < length) {
			copy_length = SKYNET_RADIO_MAX_SIZE;
		}

		skynet_packet *pkt = malloc(sizeof(skynet_packet));
		char* newdata = malloc(copy_length * sizeof(char));
		if (pkt != NULL) {
			//printf("pkt: %p\n", pkt);				// DEBUG: pointer check
			//printf("newdata: %p\n", newdata);		// DEBUG: pointer check
			memcpy(newdata, data, copy_length);
			pkt->data = newdata;
			pkt->length = copy_length;
			events_enqueue(EVENT_RF_GOT_PACKET, pkt);
		}
		else {
			// TODO: ERROR, could not malloc memory
		}


#ifdef DEBUG
		for (int j = 0; j < i; ++j) {
			DBG("status[%d] = %d %d %d\n", j, status_ph[j], status_mod[j], status_chip[j]);
		}
#endif

		vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber, 0x0);
	}

	radio_enable_irq();
}

INLINE bool radio_get_gpio0(void) {
	return Chip_GPIO_GetPinState(LPC_GPIO, SI_LIB_GPIO0_PORT, SI_LIB_GPIO0_PIN);
}

void radio_config_for_clock_measurement() {
	si446x_get_property(0x0, 1, 1);
	DBG("GLOBAL_CLK_CFG: %x\n", Si446xCmd.GET_PROPERTY.DATA0);

	Si446xCmd.GET_PROPERTY.DATA0 = 0b01110000; // enable clock output and divide by 30
	si446x_set_property_lpc(0x0, 1, 1);

	si446x_get_property(0x0, 1, 1);
	DBG("GLOBAL_CLK_CFG: %x\n", Si446xCmd.GET_PROPERTY.DATA0);

	si446x_gpio_pin_cfg(0x7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);
}

void si446x_set_property_lpc( uint8_t GROUP, uint8_t NUM_PROPS, uint8_t START_PROP)
{
	Pro2Cmd[0] = SI446X_CMD_ID_SET_PROPERTY;
	Pro2Cmd[1] = GROUP;
	Pro2Cmd[2] = NUM_PROPS;
	Pro2Cmd[3] = START_PROP;
	Pro2Cmd[4] = Si446xCmd.GET_PROPERTY.DATA0;
	Pro2Cmd[5] = Si446xCmd.GET_PROPERTY.DATA1;
	Pro2Cmd[6] = Si446xCmd.GET_PROPERTY.DATA2;
	Pro2Cmd[7] = Si446xCmd.GET_PROPERTY.DATA3;
	Pro2Cmd[8] = Si446xCmd.GET_PROPERTY.DATA4;
	Pro2Cmd[9] = Si446xCmd.GET_PROPERTY.DATA5;
	Pro2Cmd[10] = Si446xCmd.GET_PROPERTY.DATA6;
	Pro2Cmd[11] = Si446xCmd.GET_PROPERTY.DATA7;
	Pro2Cmd[12] = Si446xCmd.GET_PROPERTY.DATA8;
	Pro2Cmd[13] = Si446xCmd.GET_PROPERTY.DATA9;
	Pro2Cmd[14] = Si446xCmd.GET_PROPERTY.DATA10;
	Pro2Cmd[15] = Si446xCmd.GET_PROPERTY.DATA11;
	/*Pro2Cmd[16] = Si446xCmd.GET_PROPERTY.DATA12;
    Pro2Cmd[17] = Si446xCmd.GET_PROPERTY.DATA13;
    Pro2Cmd[18] = Si446xCmd.GET_PROPERTY.DATA14;
    Pro2Cmd[19] = Si446xCmd.GET_PROPERTY.DATA15;*/

	radio_comm_SendCmd(NUM_PROPS+4, Pro2Cmd);
}
