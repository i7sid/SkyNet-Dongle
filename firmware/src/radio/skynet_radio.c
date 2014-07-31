/**
 * @file
 *
 * @date	01.07.2014
 * @author	Michael Zapf <michael.zapf@fau.de>
 */

#include "skynet_radio.h"
#include "../cpu/systick.h"

uint8_t bMain_IT_Status;
uint8_t pwrLvlIdx = 0;
uint8_t pwrLvl[] = {8,12,19,35,127};	//0, 5, 10, 15, 20 dBm
char timeStr[] = {0, 0, 0, 0, 0, 0};


void radio_init(void) {
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
}

void radio_shutdown(void) {
	radio_set_powered(false);
}

void radio_wakeup(void) {
	radio_set_powered(true);
}

void radio_set_powered(bool on) {
	Chip_GPIO_SetPinState(LPC_GPIO, SI_LIB_SDN_PORT, SI_LIB_SDN_PIN, !on);
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
			_delay_ms(10);					//just to be sure

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

		DBG("rxPKT:");
		//for (uint8_t i = 0; i < pRadioConfiguration->Radio_PacketLength; i++)
		//{
		//	uart1_putc(customRadioPacket[i]);
		//}
		DBG("\n");

		// inform skytraxx
		bt_uart_puts_P("$1,");
		bt_uart_puts((char *) &customRadioPacket[1]);
		bt_uart_puts_P("\r\n");

		/*
		uint8_t rssi = vRadio_getFFR_A();
		char rssiStr[10];

		//do logging
		//$LOGR,RSSI,ID\r\n
		bt_uart_puts_P("$LOGR,");
		char temp[10];
		sprintf(temp, "%i,", customRadioPacket[0]);
		bt_uart_puts(temp);
		itoa(rssi, rssiStr, 10);
		bt_uart_puts(rssiStr);
		bt_uart_putc(',');
		customRadioPacket[6] = 0;
		customRadioPacket[11] = 0;
		bt_uart_puts((char *) &customRadioPacket[1]);
		bt_uart_puts((char *) &customRadioPacket[7]);
		bt_uart_puts_P("\r\n");
*/
		vRadio_StartRX(pRadioConfiguration->Radio_ChannelNumber, pRadioConfiguration->Radio_PacketLength);
		break;

	default:
		break;
	} /* switch */
}


void RADIO_IRQ_HANDLER(void) {
	// TODO: Daten nicht weiterverarbeiten, sondern einqueuen und später weiterverarbeiten
	radio_packet_handler();

	si446x_get_int_status(0u, 0u, 0u);	// update IRQ state in radio CPU
	LPC_SYSCTL->EXTINT |= (1<<EINT1);	// reset IRQ state


	//TODO???
}
