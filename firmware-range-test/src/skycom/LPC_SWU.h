/***********************************************************************
 * $Id:: i2c_example.c 1332 2008-11-19 21:20:07Z tangdz                $
 *
 * Project: LPC11XX/LPC13XX Software UART
 *
 * Description:
 * 		This file contains the interface definition for the software
 * 		UART example.
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/

#ifndef __SOFT_UART_H 
#define __SOFT_UART_H

#include "chip.h"

#define __weak					__attribute__((weak))
#define GPIOSetValue(x, y, z)	Chip_GPIO_SetPinState(LPC_GPIO, x, y, z)
#define LPC_TIMER				LPC_TIMER2

/*
#include "LPC13xx.h"
#include "system_LPC13xx.h"
*/
/*********************************************************
** Pin Definitions                                      **
*********************************************************/
#define PORT_TX_PRO (0)   //P3.0
//#define PORT_INT    (3)   //P3.1
//#define PORT_INT_TX (3)   //P3.2
//#define PORT_INT_RX (3)   //P3.3
//#define PORT_CALL   (1)   //P1.11
//#define PORT_SW_RX  (0)   //P1.5


#define PIN_TX_PRO  (8)
/*
#define PIN_INT     (1)
#define PIN_INT_TX  (2)
#define PIN_INT_RX  (3)
#define PIN_CALL    (11)
#define PIN_SW_RX   (8)
*/

/*********************************************************
** Software UART configurable parameters                **
*********************************************************/
//#define TEST_TIMER_NUM  0   /* 0 or 1 for 32-bit timers only */
#define TXBUFF_LEN      16
#define RXBUFF_LEN      16

//12000000/9600 = 1250 PCLKs
//PCLK=12MHz:
//#define BIT_LENGTH  1250
//#define BIT_LENGTH  2500

//24000000/9600 = 2500 PCLKs
//PCLK=12MHz:
//#define BIT_LENGTH  2500

//72000000/9600 = 7500 PCLKs
//PCLK=12MHz:
//#define BIT_LENGTH  7500

#define STOP_BIT_SAMPLE (9*BIT_LENGTH)
/*********************************************************
** Exported Functions                                   **
*********************************************************/
void swu_init(LPC_TIMER_T* const UART_TIMER);		//Initialization and startup
void swu_tx_str(unsigned char const*);    					//Transmit a string
void swu_tx_chr(const unsigned char);  						//Transmit a single character
unsigned char swu_rx_chr (void);  						//Read last character received

void swu_isr_tx(LPC_TIMER_T* const TX_ISR_TIMER);	//Transmit interrupt routine
void swu_isr_rx(LPC_TIMER_T* const RX_ISR_TIMER);	//Receive interrupt routine
extern void swu_rx_callback(void);       				//Call back from true RX ISR (swu_isr_rx)

#endif//__SOFT_UART_H
