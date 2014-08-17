/**
 * @file
 *
 * @date	01.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#include <string.h>

#include "skynet_radio.h"
#include "radio_config.h"
#include "radio_hal.h"
#include "../spi/spi.h"
#include "../cpu/systick.h"
#include "../misc/event_queue.h"

uint8_t bMain_IT_Status;
uint8_t pwrLvlIdx = 0;
uint8_t pwrLvl[] = {8,12,19,35,127};	//0, 5, 10, 15, 20 dBm
char timeStr[] = {0, 0, 0, 0, 0, 0};

// should be greater than RADIO_CONFIGURATION_DATA_RADIO_PACKET_LENGTH
uint8_t packet_rx_buf[RADIO_MAX_PACKET_LENGTH];

volatile bool radio_initialized = false;



void radio_init(void) {
	Chip_GPIOINT_Init(LPC_GPIOINT);
	vRadio_Init();	// intialize radio chip

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

    NVIC_SetPriority(EINT3_IRQn, 1);
    radio_enable_irq();

    // start receiving...
	vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber, pRadioConfiguration->Radio_PacketLength);
	DBG("Radio RX started.\n");
}

void radio_shutdown(void) {
	radio_disable_irq();
	radio_hal_AssertShutdown();
	SPI_Deinit();
}

void radio_enable_irq(void) {
	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT, GPIOINT_PORT0, (1 << 19));
	Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIOINT_PORT0, (1 << 19));
	NVIC_ClearPendingIRQ(EINT3_IRQn);
	NVIC_EnableIRQ(EINT3_IRQn);
}

void radio_disable_irq(void) {
	NVIC_ClearPendingIRQ(EINT3_IRQn);
	NVIC_DisableIRQ(EINT3_IRQn);
}

//TODO
void send_variable_packet(uint8_t *pkt, uint8_t length) {
	pwrLvlIdx = 0;
	DBG("txPKT\n");

	for (uint8_t pos = 0u; pos < pRadioConfiguration->Radio_PacketLength; pos++) {
		if (pos < length)
			customRadioPacket[pos+1] = pkt[pos];
		else
			customRadioPacket[pos+1] = 0;
	}

	vRadio_Change_PwrLvl(pwrLvl[pwrLvlIdx]);
	customRadioPacket[0] = pwrLvl[pwrLvlIdx];

	vRadio_StartTx_Variable_Packet(pRadioConfiguration->Radio_ChannelNumber, &customRadioPacket[0], pRadioConfiguration->Radio_PacketLength);
}

//TODO
void radio_packet_handler(void) {
	bMain_IT_Status = bRadio_Check_Tx_RX();

	switch (bMain_IT_Status)
	{
	case SI446X_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT:

		// Message "ACK" sent successfully
		DBG("txPKTk\n");

		if(++pwrLvlIdx < sizeof(pwrLvl))
		{
			vRadio_Change_PwrLvl(pwrLvl[pwrLvlIdx]);
			customRadioPacket[0] = pwrLvl[pwrLvlIdx];
			msDelayActive(10);					//just to be sure

			vRadio_StartTx_Variable_Packet(pRadioConfiguration->Radio_ChannelNumber, &customRadioPacket[0], pRadioConfiguration->Radio_PacketLength);
		}
		else
		{
			// Start RX with radio packet length
			vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber, pRadioConfiguration->Radio_PacketLength);
			//ledOff(LED_RADIO);
		}
		break;

	case SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT:
		/*
		DBG("rxPKT: ");
		for (uint8_t i = 0; i < pRadioConfiguration->Radio_PacketLength; i++)
		{
			DBG("%c", customRadioPacket[i]);
		}
		DBG("\n");
		*/

		memcpy(packet_rx_buf, customRadioPacket, pRadioConfiguration->Radio_PacketLength);
		packet_rx_buf[pRadioConfiguration->Radio_PacketLength] = '\0';


		uint8_t rssi = vRadio_getFFR_A();
		DBG("RSSI: %i\n\n", rssi);

		events_enqueue(EVENT_RF_GOT_PACKET);

		vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber, pRadioConfiguration->Radio_PacketLength);
		break;

	default:
		break;
	} /* switch */
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


void RADIO_IRQ_HANDLER(void) {
	DBG("RADIO_IRQ_HANDLER\n");
	// TODO: Daten nicht weiterverarbeiten, sondern einqueuen und später weiterverarbeiten

	if (Chip_GPIOINT_GetStatusFalling(LPC_GPIOINT, GPIOINT_PORT0) & (1 << 19)) {

		//DBG("RADIO packet received\n");
		radio_packet_handler();

//#ifndef SKYNET_TX_TEST	// DEBUG
		si446x_get_int_status(0u, 0u, 0u);	// update IRQ state in radio CPU
//#endif

		Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, GPIOINT_PORT0, (1 << 19));
	}

	LPC_SYSCTL->EXTINT |= (1<<EINT3);	// reset IRQ state
	NVIC_ClearPendingIRQ(EINT3_IRQn);
	//DBG("Leave RADIO_IRQ_HANDLER\n");
	//TODO???
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
    Pro2Cmd[16] = Si446xCmd.GET_PROPERTY.DATA12;
    Pro2Cmd[17] = Si446xCmd.GET_PROPERTY.DATA13;
    Pro2Cmd[18] = Si446xCmd.GET_PROPERTY.DATA14;
    Pro2Cmd[19] = Si446xCmd.GET_PROPERTY.DATA15;

    radio_comm_SendCmd(NUM_PROPS+4, Pro2Cmd);
}
