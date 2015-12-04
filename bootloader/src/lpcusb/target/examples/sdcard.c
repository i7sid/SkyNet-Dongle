/*
	LPCUSB, an USB device driver for LPC microcontrollers	
	Copyright (C) 2008 Bertrik Sikken (bertrik@sikken.nl)

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright
	   notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright
	   notice, this list of conditions and the following disclaimer in the
	   documentation and/or other materials provided with the distribution.
	3. The name of the author may not be used to endorse or promote products
	   derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
	IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, 
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
	THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "debug.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "spi.h"

#include "sdcard.h"

#define R1_IDLE_STATE			(1<<0)
#define R1_ERASE_RESET			(1<<1)
#define R1_ILLEGAL_COMMAND		(1<<2)
#define R1_COM_CRC_ERROR		(1<<3)
#define R1_ERASE_SEQ_ERROR		(1<<4)
#define R1_ADDRESS_ERROR		(1<<5)
#define R1_PARAMETER_ERROR		(1<<6)
#define R1_INVALID_ERROR		(1<<7)

#define R2_CARD_LOCKED			(1<<0)
#define R2_WP_ERROR				(1<<1)
#define R2_ERROR				(1<<2)
#define R2_CC_ERROR				(1<<3)
#define R2_CARD_ECC_FAILED		(1<<4)
#define R2_WP_VIOLATION			(1<<5)
#define R2_ERASE_PARAMETER		(1<<6)
#define R2_OUT_OF_RANGE			(1<<7)

#define CMD_GO_IDLE_STATE			0			
#define CMD_SEND_OP_COND			1
#define CMD_SWITCH_FUNC				6
#define CMD_SEND_IF_COND			8
#define CMD_SEND_CSD				9
#define CMD_SEND_CID				10
#define CMD_STOP_TRANSMISSION		12
#define CMD_SEND_STATUS				13
#define CMD_SET_BLOCKLEN			16
#define CMD_READ_SINGLE_BLOCK		17
#define CMD_READ_MULTIPLE_BLOCK		18
#define CMD_WRITE_BLOCK				24
#define CMD_WRITE_MULTIPLE_BLOCK	25
#define CMD_PROGRAM_CSD				27
#define CMD_SET_WRITE_PROT			28
#define CMD_CLR_WRITE_PROT			29
#define CMD_SEND_WRITE_PROT			30
#define CMD_ERASE_WR_BLK_START_ADDR	32
#define CMD_ERASE_WR_BLK_END_ADDR	33
#define CMD_ERASE					38
#define CMD_SD_SEND_OP_COND			41
#define CMD_LOCK_UNLOCK				42
#define CMD_APP_CMD					55	
#define CMD_GEN_CMD					56
#define CMD_READ_OCR				58
#define CMD_CRC_ON_OFF				59

// tokens
#define TOKEN_START_MULT_BLOCK		0xFC
#define TOKEN_STOP_TRAN				0xFD
#define TOKEN_START_BLOCK			0xFE

#define OCR_HCS						(1<<30)


#define SD_BLOCK_SIZE				512

// timing
#define	NCR			8
#define NAC			1024		// actually much more complex, TODO
#define NWR			1			// (bytes) time between write response and data block


typedef enum {
	eCardUnknown,
	eCardSDV1,
	eCardSDV2,
	eCardSDV2HC
} ECardType;

static ECardType eCardType;


// returns an R1 error code
static uint8_t SDWaitResp(int iTimeout)
{
	uint8_t	bResp;
	int	i;
	
	bResp = 0xFF;
	for (i = 0; i < iTimeout; i++) {
		SPITransfer(1, NULL, &bResp);
		if (bResp != 0xFF) {
			break;
		}
	}
	return bResp;
}

// return extra response data
static uint32_t SDExtraResp(int iLen)
{
	uint8_t	abResp[4];
	uint32_t	ulResp;
	int i;

	ASSERT(iLen <= 4);

	SPITransfer(iLen, NULL, abResp);
	ulResp = 0;
	for (i = 0; i < iLen; i++) {
		ulResp = (ulResp << 8) | abResp[i];
	}
	return ulResp;
}

// returns an R1 error code
static uint8_t SDCommand(uint8_t bCmd, uint32_t ulParam)
{
	uint8_t	abBuf[6];
	uint8_t	bResp;
	
	// check if card is busy
	SPITransfer(1, NULL, &bResp);
	if (bResp != 0xFF) {
		DBG("Card busy (0x%02X)!\n", bResp);
		return bResp;
	}
	
	// write command
	abBuf[0] = bCmd | 0x40;
	abBuf[1] = ulParam >> 24;
	abBuf[2] = ulParam >> 16;
	abBuf[3] = ulParam >> 8;
	abBuf[4] = ulParam >> 0;
	abBuf[5] = (bCmd == CMD_SEND_IF_COND) ? 0x87 : 0x95;
	SPITransfer(6, abBuf, NULL);
	
	// wait for response
	return SDWaitResp(NCR);
}

// wait for card to initialise
static bool SDSendOpCond(uint32_t ulOpCond)
{
	int i;
	uint8_t	bResp;
	
	// send ACMD41 until ready
	for (i = 0; i < 1024; i++) {
		bResp = SDCommand(CMD_APP_CMD, 0);
		bResp = SDCommand(CMD_SD_SEND_OP_COND, ulOpCond);
		if (bResp == 0) {
			return true;
		}
	}
	DBG("CMD_SEND_OP_COND failed\n");
	return false;
}


static bool SDReadDataToken(uint8_t bType, uint8_t *pbData, int iLen)
{
	uint8_t	bResp;

	// wait for data token
	bResp = SDWaitResp(NAC);
	if (bResp != bType) {
		DBG("Expected start block token, got %X instead!\n", bResp);
		return false;
	}
	
	// read data
	SPITransfer(iLen, NULL, pbData);
	
	// skip CRC
	SPITransfer(2, NULL, NULL);
	
	return true;
}


static bool SDWriteDataToken(uint8_t bType, const uint8_t *pbData, int iLen)
{
	uint8_t	bResp;

	// NWR
	SPITransfer(1, NULL, NULL);

	// data token
	SPITransfer(1, &bType, NULL);
	if (iLen != 0) {
		// send data
	SPITransfer(iLen, pbData, NULL);
	// (fake) CRC
	SPITransfer(2, NULL, NULL);
	// get data response
	SPITransfer(1, NULL, &bResp);
	if ((bResp & 0x1F) != 5) {
		DBG("Received data response error (0x%02X)!\n", bResp);
		return false;
	}
	}
	
	// wait while busy
	do {
		SPITransfer(1, NULL, &bResp);
	} while (bResp != 0xFF);
	
	return true;
}


bool SDInit(void)
{
	int i;
	uint8_t	bResp;
	uint32_t	ulOCR;
	uint32_t ulData;

	eCardType = eCardUnknown;

	// init SPI subsystem
	SPIInit();

	// set low SPI speed
	SPISetSpeed(400000);

	// send at least 74 clocks with no chip select
	SPITick(10);

	// send CMD_GO_IDLE_STATE
	for (i = 0; i < 100; i++) {
		bResp = SDCommand(CMD_GO_IDLE_STATE, 0);
		if (bResp != 0xFF) {
			break;
		}
	}
	if (bResp != R1_IDLE_STATE) {
		DBG("CMD_GO_IDLE_STATE failed (0x%02X)!\n", bResp);
		return false;
	}
	
	// send CMD8
	bResp = SDCommand(CMD_SEND_IF_COND, 0x1AA);
	if (bResp == 1) {
		// Ver2.00 or later SD memory card
		ulData = SDExtraResp(4);
		if (ulData != 0x1AA) {
			DBG("CMD8 data 0x%08X\n", ulData);
			return false;
		}

		if (!SDSendOpCond(OCR_HCS)) {
			DBG("SDSendOpCond failed!\n");
			return false;
		}
		
		// Check CCS bit
		if (!SDReadOCR(&ulOCR)) {
			DBG("ReadOCR failed!\n");
			return false;
		}
		eCardType = ((ulOCR & OCR_HCS) != 0) ? eCardSDV2HC : eCardSDV2;
	}
	else {
		// Ver1.X SD memory card or other
		if (!SDSendOpCond(0)) {
			DBG("SDSendOpCond failed!\n");
		return false;
	}
		eCardType = eCardSDV1;
	}
	
	// set high SPI speed
	SPISetSpeed(25000000);

	return true;
}


static uint32_t SDBlock2Addr(uint32_t ulBlock)
{
	return (eCardType == eCardSDV2HC) ? ulBlock : ulBlock * SD_BLOCK_SIZE;
}


bool SDReadBlock(uint8_t *pbData, uint32_t ulBlock)
{
	uint32_t	ulAddr;
	uint8_t	bResp;
	
	// calculate card address
	ulAddr = SDBlock2Addr(ulBlock);
	
	// write command
	if ((bResp = SDCommand(CMD_READ_SINGLE_BLOCK, ulAddr)) != 0) {
		DBG("CMD_READ_SINGLE_BLOCK failed (0x%02X)!\n", bResp);
		return false;
	}
	
	// read data token
	if (!SDReadDataToken(TOKEN_START_BLOCK, pbData, SD_BLOCK_SIZE)) {
		DBG("SDReadDataToken failed!\n");
		return false;
	}
	
	return true;
}



bool SDWriteBlock(const uint8_t *pbData, uint32_t ulBlock)
{
	uint32_t	ulAddr;
	uint8_t	bResp;
	
	// calculate card address
	ulAddr = SDBlock2Addr(ulBlock);
	
	// write command
	if ((bResp = SDCommand(CMD_WRITE_BLOCK, ulAddr)) != 0) {
		DBG("CMD_WRITE_BLOCK failed (0x%02X)!\n", bResp);
		return false;
	}
	
	// write data token
	if (!SDWriteDataToken(TOKEN_START_BLOCK, pbData, SD_BLOCK_SIZE)) {
		DBG("SDWriteDataToken failed!\n");
		return false;
	}
	
	return true;
}


bool SDReadCSD(uint8_t *pbCSD)
{
	uint8_t	bResp;

	// write command
	if ((bResp = SDCommand(CMD_SEND_CSD, 0)) != 0) {
		DBG("CMD_SEND_CSD failed (0x%02X)!\n", bResp);
		return false;
	}
	
	// wait for data token
	if (!SDReadDataToken(TOKEN_START_BLOCK, pbCSD, 16)) {
		DBG("SDReadDataToken failed!\n");
		return false;
	}
	
	return true;
}
	
	
bool SDReadCID(uint8_t *pbCID)
{
	uint8_t	bResp;

	// write command
	if ((bResp = SDCommand(CMD_SEND_CID, 0)) != 0) {
		DBG("CMD_SEND_CID failed (0x%02X)!\n", bResp);
		return false;
	}
	
	// wait for data token
	if (!SDReadDataToken(TOKEN_START_BLOCK, pbCID, 16)) {
		DBG("SDReadDataToken failed!\n");
		return false;
	}

	return true;
}
	

bool SDReadOCR(uint32_t *pulOCR)
{
	uint8_t	bResp;

	// write command
	if ((bResp = SDCommand(CMD_READ_OCR, 0)) != 0) {
		DBG("CMD_READ_OCR failed (0x%02X)!\n", bResp);
		return false;
	}
	*pulOCR = SDExtraResp(4);
	return true;
}


