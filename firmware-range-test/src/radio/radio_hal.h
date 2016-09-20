/*!
 * File:
 *  radio_hal.h
 *
 * Description:
 *  This file contains RADIO HAL.
 *
 * Silicon Laboratories Confidential
 * Copyright 2011 Silicon Laboratories, Inc.
 */

#ifndef _RADIO_HAL_H_
#define _RADIO_HAL_H_


#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif



void radio_hal_AssertShutdown(void);
void radio_hal_DeassertShutdown(void);
void radio_hal_ClearNsel(void);
void radio_hal_SetNsel(void);
uint8_t radio_hal_NirqLevel(void);

void radio_hal_SpiWriteByte(uint8_t byteToWrite);
uint8_t radio_hal_SpiReadByte(void);

void radio_hal_SpiWriteData(uint8_t byteCount, uint8_t* pData);
void radio_hal_SpiReadData(uint8_t byteCount, uint8_t* pData);

#ifdef DRIVERS_EXTENDED_SUPPORT
  BIT radio_hal_Gpio0Level(void);
  BIT radio_hal_Gpio1Level(void);
  BIT radio_hal_Gpio2Level(void);
  BIT radio_hal_Gpio3Level(void);
#endif

#endif //_RADIO_HAL_H_
