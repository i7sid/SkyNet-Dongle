/*
	LPCUSB, an USB device driver for LPC microcontrollers
	Copyright (C) 2006 Bertrik Sikken (bertrik@sikken.nl)

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

/**	@file

	Bulk-only-transfer layer for mass storage.

	This layers sits between the generic USB layers and the SCSI layer
	and performs data transfer according to the BOT protocol.
*/

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "debug.h"

#include "usbapi.h"

#include "msc_bot.h"
#include "msc_scsi.h"


/** Command block wrapper structure */
typedef struct {
	uint32_t		dwCBWSignature;
	uint32_t		dwCBWTag;
	uint32_t		dwCBWDataTransferLength;
	uint8_t		bmCBWFlags;
	uint8_t		bCBWLun;
	uint8_t		bCBWCBLength;
	uint8_t		CBWCB[16];
} TCBW;

/** Command status wrapper structure */
typedef struct {
	uint32_t		dwCSWSignature;
	uint32_t		dwCSWTag;
	uint32_t		dwCSWDataResidue;
	uint8_t		bmCSWStatus;
} TCSW;

/** States of BOT state machine */
typedef enum {
	eCBW,
	eDataOut,
	eDataIn,
	eCSW,
	eStalled
} EBotState;

#define MAX(x,y)       ((x)>(y)?(x):(y))       /**< MAX */
#ifndef MIN
#define MIN(a,b)	((a)<(b)?(a):(b))
#endif

#define CBW_SIGNATURE	0x43425355		/**< magic word in CBW */
#define CSW_SIGNATURE	0x53425355		/**< magic word in CSW */

#define STATUS_PASSED		0x00		/**< successful transfer */
#define STATUS_FAILED		0x01		/**< failed transfer */
#define STATUS_PHASE_ERR	0x02		/**< conflict between host and device */

static uint32_t			dwTransferSize;		/**< total size of data transfer */
static uint32_t			dwOffset;			/**< offset in current data transfer */

static TCBW			CBW;
static TCSW			CSW;

static EBotState	eState;

static uint8_t			*pbData;



/**
	Resets the BOT state machine
 */
void MSCBotReset(void)
{
	DBG("BOT reset in state %d\n", eState);
	// reset BOT state
	eState = eCBW;
	// reset SCSI
	SCSIReset();
}


/**
	Prepares a CSW, to be sent on next bulk-IN interrupt

	@param [in]	bStatus	CSW status
 */
static void SendCSW(uint8_t bStatus)
{
	int iResidue;

	iResidue = CBW.dwCBWDataTransferLength - dwTransferSize;

	// construct CSW
	CSW.dwCSWSignature		= CSW_SIGNATURE;
	CSW.dwCSWTag 			= CBW.dwCBWTag;
	CSW.dwCSWDataResidue 	= MAX(iResidue, 0);
	CSW.bmCSWStatus 		= bStatus;

	DBG("CSW: status=%x, residue=%d\n", bStatus, CSW.dwCSWDataResidue);

	// next state
	eState = eCSW;
}


/**
	Checks if CBW is valid and meaningful

	@param [in]	pCBW	Command block wrapper
	@param [in]	iLen	Length of CBW

	@return true if valid and meaningful
 */
static bool CheckCBW(TCBW *pCBW, int iLen)
{
	// CBW valid?
	if (iLen != 31) {
		DBG("Invalid length (%d)\n", iLen);
		return false;
	}
	if (pCBW->dwCBWSignature != CBW_SIGNATURE) {
		DBG("Invalid signature %x\n", pCBW->dwCBWSignature);
		return false;
	}

	// CBW meaningful?
	if (pCBW->bCBWLun != 0) {
		DBG("Invalid LUN %d\n", pCBW->bCBWLun);
		return false;
	}
	if ((pCBW->bCBWCBLength < 1) || (pCBW->bCBWCBLength > 16)) {
		DBG("Invalid CB len %d\n", pCBW->bCBWCBLength);
		return false;
	}
	return true;
}


/*************************************************************************
	BOTStall
	========
		Local function to stall ongoing transfer

	Which endpoint to stall is determined by looking at the transfer
	direction intended by the host.

**************************************************************************/
static void BOTStall(void)
{
	if ((CBW.bmCBWFlags & 0x80) || (CBW.dwCBWDataTransferLength == 0)) {
		// stall data-in or CSW
		USBHwEPStall(MSC_BULK_IN_EP, true);
	}
	else {
		// stall data-out
		USBHwEPStall(MSC_BULK_OUT_EP, true);
	}
}


/*************************************************************************
	HandleDataIn
	============
		Handles data from device-to-host

**************************************************************************/
static void HandleDataIn(void)
{
	int iChunk;

	// process data for host in SCSI layer
	pbData = SCSIHandleData(CBW.CBWCB, CBW.bCBWCBLength, pbData, dwOffset);
	if (pbData == NULL) {
		BOTStall();
		SendCSW(STATUS_FAILED);
		return;
	}

	// send data to host?
	if (dwOffset < dwTransferSize) {
		iChunk = MIN(64, dwTransferSize - dwOffset);
		USBHwEPWrite(MSC_BULK_IN_EP, pbData, iChunk);
		dwOffset += iChunk;
	}

	// are we done now?
	if (dwOffset == dwTransferSize) {
		if (dwOffset != CBW.dwCBWDataTransferLength) {
			// stall pipe
			DBG("stalling DIN");
			BOTStall();
		}
		// done
		SendCSW(STATUS_PASSED);
	}
}


/*************************************************************************
	HandleDataOut
	=============
		Handles data from host-to-device

**************************************************************************/
static void HandleDataOut(void)
{
	int iChunk;

	if (dwOffset < dwTransferSize) {
		// get data from host
		iChunk = USBHwEPRead(MSC_BULK_OUT_EP, pbData, dwTransferSize - dwOffset);
		// process data in SCSI layer
		pbData = SCSIHandleData(CBW.CBWCB, CBW.bCBWCBLength, pbData, dwOffset);
		if (pbData == NULL) {
			BOTStall();
			SendCSW(STATUS_FAILED);
			return;
		}
		dwOffset += iChunk;
	}

	// are we done now?
	if (dwOffset == dwTransferSize) {
		if (dwOffset != CBW.dwCBWDataTransferLength) {
			// stall pipe
			DBG("stalling DOUT");
			BOTStall();
		}
		SendCSW(STATUS_PASSED);
	}
}


/**
	Handles the BOT bulk OUT endpoint

	@param [in]	bEP			Endpoint number
	@param [in]	bEPStatus	Endpoint status (indicates NAK, STALL, etc)

 */
void MSCBotBulkOut(uint8_t bEP, uint8_t bEPStatus)
{
	int 	iLen, iChunk;
	bool	fHostIn, fDevIn;

	// ignore events on stalled EP
	if (bEPStatus & EP_STATUS_STALLED) {
		return;
	}

	switch (eState) {

	case eCBW:
		iLen = USBHwEPRead(bEP, (uint8_t *)&CBW, sizeof(CBW));

		// check if we got a good CBW
		if (!CheckCBW(&CBW, iLen)) {
			// see 6.6.1
			USBHwEPStall(MSC_BULK_IN_EP, true);
			USBHwEPStall(MSC_BULK_OUT_EP, true);
			eState = eStalled;
			break;
		}

		DBG("CBW: len=%d, flags=%x, cmd=%x, cmdlen=%d\n",
			CBW.dwCBWDataTransferLength, CBW.bmCBWFlags, CBW.CBWCB[0], CBW.bCBWCBLength);

		dwOffset = 0;
		dwTransferSize = 0;
		fHostIn = ((CBW.bmCBWFlags & 0x80) != 0);

		// verify request
		pbData = SCSIHandleCmd(CBW.CBWCB, CBW.bCBWCBLength, &iLen, &fDevIn);
		if (pbData == NULL) {
			// unknown command
			BOTStall();
			SendCSW(STATUS_FAILED);
			break;
		}

		// rule: if device and host disagree on direction, send CSW with status 2
		if ((iLen > 0) &&
			((fHostIn && !fDevIn) ||
			(!fHostIn && fDevIn))) {
			DBG("Host and device disagree on direction\n");
			BOTStall();
			SendCSW(STATUS_PHASE_ERR);
			break;
		}

		// rule: if D > H, send CSW with status 2
		if (iLen > CBW.dwCBWDataTransferLength) {
			DBG("Negative residue\n");
			BOTStall();
			SendCSW(STATUS_PHASE_ERR);
			break;
		}

		dwTransferSize = iLen;
		if ((dwTransferSize == 0) || fDevIn) {
			// data from device-to-host
			eState = eDataIn;
			HandleDataIn();
		}
		else {
			// data from host-to-device
			eState = eDataOut;
		}
		break;

	case eDataOut:
		HandleDataOut();
		break;

	case eDataIn:
	case eCSW:
		iChunk = USBHwEPRead(bEP, NULL, 0);
		DBG("Phase error in state %d, %d bytes\n", eState, iChunk);
		eState = eCBW;
		break;

	case eStalled:
		// keep stalling
		USBHwEPStall(MSC_BULK_OUT_EP, true);
		break;

	default:
		DBG("Invalid state %d\n", eState);
		ASSERT(false);
		break;
	}
}


/**
	Handles the BOT bulk IN endpoint

	@param [in]	bEP			Endpoint number
	@param [in]	bEPStatus	Endpoint status (indicates NAK, STALL, etc)

 */
void MSCBotBulkIn(uint8_t bEP, uint8_t bEPStatus)
{
	// ignore events on stalled EP
	if (bEPStatus & EP_STATUS_STALLED) {
		return;
	}

	switch (eState) {

	case eCBW:
	case eDataOut:
		// ignore possibly old ACKs
		break;

	case eDataIn:
		HandleDataIn();
		break;

	case eCSW:
		// wait for an IN token, then send the CSW
		USBHwEPWrite(MSC_BULK_IN_EP, (uint8_t *)&CSW, 13);
		eState = eCBW;
		break;

	case eStalled:
		// keep stalling
		USBHwEPStall(MSC_BULK_IN_EP, true);
		break;

	default:
		DBG("Invalid state %d\n", eState);
		ASSERT(false);
		break;
	}
}


