/*! @file radio.h
 * @brief This file is contains the public radio interface functions.
 *
 * @b COPYRIGHT
 * @n Silicon Laboratories Confidential
 * @n Copyright 2012 Silicon Laboratories, Inc.
 * @n http://www.silabs.com
 */

#ifndef RADIO_H_
#define RADIO_H_

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif




/*! Maximal packet length definition (FIFO size) */
#define RADIO_MAX_PACKET_LENGTH     	64
//#define RADIO_RX_ALMOST_FULL_THRESHOLD 	SI446X_PROP_PKT_TX_THRESHOLD_DEFAULT
//#define RADIO_TX_ALMOST_EMPTY_THRESHOLD SI446X_PROP_PKT_RX_THRESHOLD_DEFAULT
#define RADIO_RX_ALMOST_FULL_THRESHOLD 	0x20 // 0x20
#define RADIO_TX_ALMOST_EMPTY_THRESHOLD 0x10

typedef struct
{
    uint8_t   *Radio_ConfigurationArray;

    uint8_t   Radio_ChannelNumber;
    uint8_t   Radio_PacketLength;
    uint8_t   Radio_State_After_Power_Up;

    uint16_t  Radio_Delay_Cnt_After_Reset;

//    uint8_t   Radio_CustomPayload[RADIO_MAX_PACKET_LENGTH];
    uint8_t   *Radio_Custom_Long_Payload;
} tRadioConfiguration;

extern tRadioConfiguration *pRadioConfiguration;
extern uint8_t customRadioPacket[RADIO_MAX_PACKET_LENGTH];

/*! Si446x configuration array */
extern uint8_t Radio_Configuration_Data_Array[];


extern uint8_t bPositionInPayload;
extern uint8_t* pPositionInPayload;


void	vRadio_Init(void);
uint8_t	bRadio_Check_Tx_RX(void);
void  	vRadio_StartRX(uint8_t,uint8_t);
void  	vRadio_StartRXlong(uint8_t);
uint8_t	bRadio_Check_Ezconfig(uint16_t);
void	vRadio_StartTx_Variable_Packet(uint8_t, uint8_t*, uint8_t);

uint8_t vRadio_getFFR_A(void);
void vRadio_Change_PwrLvl(uint8_t lvl);

#endif /* RADIO_H_ */
