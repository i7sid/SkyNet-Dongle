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

#include "sdcard.h"
#include "blockdev.h"

static uint32_t getsdbits(uint8_t *buf, int offset, int len)
{
	uint32_t		mask, data;
	int		bytepos, bitpos;
	int		shift;
	
	offset = 127 - offset;
	bytepos = offset / 8;
	bitpos = offset % 8;
	mask = (1 << len) - 1;

	data = 0;
	for (shift = -(len + bitpos); shift < 0; shift += 8) {
		data = (data << 8) | buf[bytepos++];
	}
	return (data >> shift) & mask;
}


bool BlockDevInit(void)
{
	return SDInit();
}


bool BlockDevWrite(uint32_t dwBlock, uint8_t* pbBuf)
{
	return SDWriteBlock(pbBuf, dwBlock);
}


bool BlockDevRead(uint32_t dwBlock, uint8_t* pbBuf)
{
	return SDReadBlock(pbBuf, dwBlock);
}


bool BlockDevGetSize(uint32_t *pdwDriveSize)
{
	uint8_t	abBuf[16];
	uint32_t	c_size, num_blocks, block_size;
	uint16_t	read_bl_len;
	uint8_t	csd_structure, c_size_mult; 
	
	/* read CSD to determine block size and number of blocks */
	if (!SDReadCSD(abBuf)) {
		return false;
	}
	csd_structure =	getsdbits(abBuf, 127, 2);
	switch (csd_structure) {
	
	case 0:
		read_bl_len =	getsdbits(abBuf, 83, 4);
		c_size =		getsdbits(abBuf, 73, 12);
		c_size_mult =	getsdbits(abBuf, 49, 3);
		num_blocks = (c_size + 1) * (4 << c_size_mult);
		block_size = 1 << read_bl_len;
		break;

	case 1:
		c_size =		getsdbits(abBuf, 69, 22);
		num_blocks = (c_size + 1) * 512 * 1024;
		block_size = 512;
		break;
		
	default:
		DBG("Invalid CSD structure (%d)!\n", csd_structure);
		return false;
	}

	*pdwDriveSize = num_blocks * block_size;
	return true;
}

