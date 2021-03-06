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
#include "../spi/spi.h"
#include "../cpu/systick.h"


//const uint8_t Radio_Configuration_Data_Custom_Long_Payload_Array[] =
//		RADIO_CONFIGURATION_DATA_CUSTOM_LONG_PAYLOAD;

uint8_t bPositionInPayload = 0u;
uint8_t* pPositionInPayload;

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

	msDelayActive(100);
}


#define RADIO_CONFIGURATION_USE_PATCH

/*!
 *  Radio Initialization.
 *
 *  @author Sz. Papp, Michael Zapf <michael.zapf@fau.de>
 *
 *  @note
 *
 */
void vRadio_Init(void)
{
	//DBG("Initialize SPI...\n");
	SPI_Init();

	// initialize nIRQ and SDN
	DBG("Setup port pins for SPI additions...\n");
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, SI_LIB_nIRQ_PORT, SI_LIB_nIRQ_PIN);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, SI_LIB_SDN_PORT, SI_LIB_SDN_PIN);


	/* Power Up the radio chip */
	DBG("Power up radio...\n");
	vRadio_PowerUp();


#ifdef RADIO_CONFIGURATION_USE_PATCH
	uint16_t wDelay;
	while(SI446X_SUCCESS != si446x_apply_patch()) {
		for (wDelay = 0x7FFF; wDelay--; ) ;
		/* Power Up the radio chip */
		vRadio_PowerUp();
	}
#endif


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
		{	// Packet RX

			// Get payload length
			si446x_fifo_info(0x00);

			si446x_read_rx_fifo(Si446xCmd.FIFO_INFO.RX_FIFO_COUNT, &customRadioPacket[0]);

			return SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT;
		}

		if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_CRC_ERROR_BIT)
		{
			DBG("crc err\n");

			// reset RX FIFO
			si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);

			// manually go to RX state again
			// (automatic transission does not work for some reason...)
			si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_RX);
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
	si446x_start_rx(channel, 0u, packetLenght,
			SI446X_CMD_START_RX_ARG_RXTIMEOUT_STATE_ENUM_NOCHANGE,	// state after preamble timeout
			SI446X_CMD_START_RX_ARG_RXVALID_STATE_ENUM_READY,		// state after PACKET_RX interrupt / valid CRC
			SI446X_CMD_START_RX_ARG_RXINVALID_STATE_ENUM_RX);		// state after invalid CRC detected
}

/*!
 *  Set Radio to TX mode, fixed packet length.
 *
 *  @param channel Freq. Channel, Packet to be sent
 *
 *  @note
 *
 */
void  vRadio_StartTx(U8 channel, U8 *pioFixRadioPacket)
{
  /* Reset TX FIFO */
  si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_TX_BIT);

  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);

  /* Position to the very beginning of the custom long payload */
  pPositionInPayload = pioFixRadioPacket;

  /* Fill the TX fifo with datas */
  if( RADIO_MAX_PACKET_LENGTH < RadioConfiguration.Radio_PacketLength)
  {
    /* Data to be sent is more than the size of TX FIFO */
    si446x_write_tx_fifo(RADIO_MAX_PACKET_LENGTH, pPositionInPayload);

    /* Calculate how many bytes are sent to TX FIFO */
    bPositionInPayload += RADIO_MAX_PACKET_LENGTH;

    /* Position to the next first byte that can be sent to TX FIFO in next round */
    pPositionInPayload += RADIO_MAX_PACKET_LENGTH;
  }
  else
  {
    // Data to be sent is less or equal than the size of TX FIFO
    si446x_write_tx_fifo(RadioConfiguration.Radio_PacketLength, pioFixRadioPacket);
  }


  /* Start sending packet, channel 0, START immediately, Packet length according to PH, go READY when done */
  si446x_start_tx(channel, 0x30,  0x00);
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
