/*! @file radio.c
 * @brief This file contains functions to interface with the radio chip.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#include <stdio.h>

#include "radio.h"
#include "radio_config.h"
#include "radio_comm.h"
#include "si446x_api_lib.h"
#include "si446x_defs.h"

#include "../misc/misc.h"
#include "../skynet.h"
#include "../spi/spi.h"

uint8_t Radio_Configuration_Data_Array[] = RADIO_CONFIGURATION_DATA_ARRAY;

tRadioConfiguration RadioConfiguration = RADIO_CONFIGURATION_DATA;
tRadioConfiguration *pRadioConfiguration = &RadioConfiguration;

uint8_t customRadioPacket[RADIO_MAX_PACKET_LENGTH];

/*!
 *  Power up the Radio.
 *
 *  @note
 *
 */
void vRadio_PowerUp(void)
{

	/* Hardware reset the chip */
	si446x_reset();

	msDelay(200);
}

/*!
 *  Radio Initialization.
 *
 *  @author Sz. Papp
 *
 *  @note
 *
 */
void vRadio_Init(void)
{
	DBG("Initialize SPI...\n");
	SPI_Init();

	//TODO: Initialisiere nIRQ und SDN?
	DBG("Setup port pins for SPI additions...\n");
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, SI_LIB_nIRQ_PORT, SI_LIB_nIRQ_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, SI_LIB_SDN_PORT, SI_LIB_SDN_PIN);

	//Chip_IOCON_PinMux(LPC_IOCON, SI_LIB_SDN_PORT, SI_LIB_SDN_PIN, IOCON_MODE_INACT, IOCON_FUNC0);




	/*
	INPUT(SI_LIB_nIRQ);
	SET(SI_LIB_nIRQ);

	OUTPUT(SI_LIB_SDN);
	SET(SI_LIB_SDN);
	*/

	/* Power Up the radio chip */
	DBG("Power up radio...\n");
	vRadio_PowerUp();

	/* Load radio configuration */
	while (SI446X_SUCCESS != si446x_configuration_init(pRadioConfiguration->Radio_ConfigurationArray))
	{
		DBG("SI446X chip init error\n");

		/* Power Up the radio chip */
		vRadio_PowerUp();
	}

//  si446x_ircal(SI446X_CMD_IRCAL_ARG_INITIAL_PH_AMP_MASK | SI446X_CMD_IRCAL_ARG_FINE_STEP_SIZE_MASK | SI446X_CMD_IRCAL_ARG_COARSE_STEP_SIZE_MASK,
//		  SI446X_CMD_IRCAL_ARG_RSSI_FINE_AVG_MASK | SI446X_CMD_IRCAL_ARG_RSSI_COARSE_AVG_MASK,
//		  SI446X_CMD_IRCAL_ARG_EN_HRMNIC_GEN_MASK | SI446X_CMD_IRCAL_ARG_RF_SOURCE_PWR_MASK | ...);


	// Read ITs, clear pending ones
	si446x_get_int_status(0u, 0u, 0u);





	Si446xCmd.GET_PROPERTY.DATA0 = 0x01;
	si446x_set_property_lpc(0x01, 1, 0);
	Si446xCmd.GET_PROPERTY.DATA0 = 0x00; 	// DEBUG
	si446x_get_property(0x01, 1, 0);		// DEBUG
    DBG("0x%x\n", Si446xCmd.GET_PROPERTY.DATA0); // DEBUG



	// configure external intrrupt pin EINT1
	//Chip_IOCON_PinMux(LPC_IOCON, SI_LIB_nIRQ_PORT, SI_LIB_nIRQ_PIN, IOCON_MODE_INACT, IOCON_FUNC1);
    //LPC_SYSCTL->EXTMODE = 0x0;
    //LPC_SYSCTL->EXTPOLAR = 0x0;


	// TODO: Interrupt an nIRQ registieren und maskieren?
    //LPC_SYSCTL->EXTINT |= 0x2;
	//NVIC_EnableIRQ(EINT1_IRQn);


	DBG("init done\n");
}

uint8_t vRadio_getFFR_A(void)
{
	si446x_frr_a_read(1);
	return Si446xCmd.FRR_A_READ.FRR_A_VALUE;
}

/*!
 *  Check if Packet sent IT flag or Packet Received IT is pending.
 *
 *  @return   SI4455_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT / SI4455_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT
 *
 *  @note
 *
 */
U8 bRadio_Check_Tx_RX(void)
{
	//if (!GET(SI_LIB_nIRQ))
	if (!PINGET(SI_LIB_nIRQ_PORT, SI_LIB_nIRQ_PIN))
	{
		// Read ITs, clear pending ones
		si446x_get_int_status(0u, 0u, 0u);
		if (Si446xCmd.GET_INT_STATUS.CHIP_PEND & SI446X_CMD_GET_CHIP_STATUS_REP_CMD_ERROR_PEND_BIT)
		{
			DBG("cmd err\n");
			// State change to
			si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);

			// Reset FIFO
			si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);

			// State change to
			si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_RX);
		}

		if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT)
		{
			DBG("pkt sent\n");
			return SI446X_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT;
		}

		if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT)
		{
			// Packet RX
			DBG("rx\n");

			// Get payload length
			si446x_fifo_info(0x00);

			si446x_read_rx_fifo(Si446xCmd.FIFO_INFO.RX_FIFO_COUNT, &customRadioPacket[0]);

			return SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT;
		}

		if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_CRC_ERROR_BIT)
		{
			DBG("crc err\n");
			//TODO?
			/*
			bt_uart_puts_P("$LOGC,");
			bt_uart_puts(timeStr);
			bt_uart_putc(',');
			for (uint8_t i = 0; i < pRadioConfiguration->Radio_PacketLength; i++)
			{
				if(customRadioPacket[i] >= 32 && customRadioPacket[i] <= 126 && customRadioPacket[i] != 92)		//'\\'
				{
					bt_uart_putc(customRadioPacket[i]);
				}
				else
				{
					bt_uart_putc(92);
					if(customRadioPacket[i] == 92)
					{
						bt_uart_putc(92);
					}
					else
					{
						char temp[10];
						sprintf(temp, "%02X", customRadioPacket[i]);
						bt_uart_puts(temp);
					}
				}
			}
			bt_uart_puts_P("\r\n");
			*/

			// Reset FIFO
			si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
		}

	}

	return 0;
}

/*!
 *  Set Radio to RX mode. .
 *
 *  @param channel Freq. Channel,  packetLength : 0 Packet handler fields are used , nonzero: only Field1 is used
 *
 *  @note
 *
 */
void vRadio_StartRX(U8 channel, U8 packetLenght)
{
	// Read ITs, clear pending ones
	si446x_get_int_status(0u, 0u, 0u);

	// Reset the Rx Fifo
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);

	/* Start Receiving packet, channel 0, START immediately, Packet length used or not according to packetLength */
	si446x_start_rx(channel, 0u, packetLenght, SI446X_CMD_START_RX_ARG_RXTIMEOUT_STATE_ENUM_NOCHANGE,
			SI446X_CMD_START_RX_ARG_RXVALID_STATE_ENUM_READY, SI446X_CMD_START_RX_ARG_RXINVALID_STATE_ENUM_RX);
}

/*!
 *  Set Radio to TX mode, variable packet length.
 *
 *  @param channel Freq. Channel, Packet to be sent length of of the packet sent to TXFIFO
 *
 *  @note
 *
 */
void vRadio_StartTx_Variable_Packet(U8 channel, U8 *pioRadioPacket, U8 length)
{
	/* Leave RX state */
	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_READY);

	/* Read ITs, clear pending ones */
	si446x_get_int_status(0u, 0u, 0u);

	/* Reset the Tx Fifo */
	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_TX_BIT);

	/* Fill the TX fifo with datas */
	si446x_write_tx_fifo(length, pioRadioPacket);

	/* Start sending packet, channel 0, START immediately */
	si446x_start_tx(channel, 0x80, length);

}


void vRadio_Change_PwrLvl(uint8_t lvl)
{
	/* Change state to ready */
	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_READY);

	/* Read ITs, clear pending ones */
	si446x_get_int_status(0u, 0u, 0u);

	si446x_pa_pwr_lvl(lvl);

	DBG("txPwr: %i\n", lvl);
}



// addition by MZ
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
