/**
 * @file
 *
 * @date		25.10.2015
 * @author		Michael Zapf <michael.zapf@fau.de>
 *
 * @note
 * This file is mostly based on example code from NXP Semiconductors.
 * The code is used under the following license:
 *
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 *
 */

#include "skynet_cdc.h"
#include "../cpu/systick.h"


static USBD_HANDLE_T g_hUsb;

extern const  USBD_HW_API_T hw_api;
extern const  USBD_CORE_API_T core_api;
extern const  USBD_CDC_API_T cdc_api;
/* Since this example only uses CDC class link functions for that clas only */
static const  USBD_API_T g_usbApi = {
	&hw_api,
	&core_api,
	0,
	0,
	0,
	&cdc_api,
	0,
	0x02221101,
};

const  USBD_API_T *g_pUsbApi = &g_usbApi;

static uint8_t current_seqno = 0;

#define USB_TX_FIFO_QUEUE_SIZE 	(4096)

static RINGBUFF_T usb_tx_ringbuf;
static char usb_tx_buf[USB_TX_FIFO_QUEUE_SIZE * sizeof(uint8_t)];


int skynet_cdc_init(void) {
	USBD_API_INIT_PARAM_T usb_param;
	USB_CORE_DESCS_T desc;
	ErrorCode_t ret = LPC_OK;

	RingBuffer_Init(&usb_tx_ringbuf, usb_tx_buf, sizeof(uint8_t), USB_TX_FIFO_QUEUE_SIZE);
	RingBuffer_Flush(&usb_tx_ringbuf);

	Chip_USB_Init();

	LPC_USB->USBClkCtrl = 0x12;                /* Dev, AHB clock enable */
	while ((LPC_USB->USBClkSt & 0x12) != 0x12);

	/* initialize call back structures */
	memset((void *) &usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
	usb_param.usb_reg_base = LPC_USB_BASE + 0x200;
	usb_param.max_num_ep = 3;
	usb_param.mem_base = USB_STACK_MEM_BASE;
	usb_param.mem_size = USB_STACK_MEM_SIZE;

	/* Set the USB descriptors */
	desc.device_desc = (uint8_t *) &USB_DeviceDescriptor[0];
	desc.string_desc = (uint8_t *) &USB_StringDescriptor[0];
	/* Note, to pass USBCV test full-speed only devices should have both
	   descriptor arrays point to same location and device_qualifier set to 0.
	 */
	desc.high_speed_desc = (uint8_t *) &USB_FsConfigDescriptor[0];
	desc.full_speed_desc = (uint8_t *) &USB_FsConfigDescriptor[0];
	desc.device_qualifier = 0;

	/* USB Initialization */
	ret = USBD_API->hw->Init(&g_hUsb, &desc, &usb_param);
	if (ret == LPC_OK) {

		/* Init VCOM interface */
		ret = vcom_init(g_hUsb, &desc, &usb_param);
		if (ret == LPC_OK) {
			/*  enable USB interrupts */
			NVIC_EnableIRQ(USB_IRQn);

			// first disable to simulate re-plug
			skynet_cdc_disconnect();

			msDelay(100);

			// and now enable connection
			skynet_cdc_connect();

			return 1;
		}
	}

	//char* welcome_msg = "# This is skynet dongle.\n";
	//vcom_write(welcome_msg, strlen(welcome_msg));
	return 0;
}

void skynet_cdc_connect(void) {
	USBD_API->hw->Connect(g_hUsb, 1);
}

void skynet_cdc_disconnect(void) {
	USBD_API->hw->Connect(g_hUsb, 0);
}



/**
 * @brief	Handle interrupt from USB0
 * @return	Nothing
 */
void USB_IRQHandler(void)
{
	USBD_API->hw->ISR(g_hUsb);
}

/* Find the address of interface descriptor for given class type. */
USB_INTERFACE_DESCRIPTOR *find_IntfDesc(const uint8_t *pDesc, uint32_t intfClass)
{
	USB_COMMON_DESCRIPTOR *pD;
	USB_INTERFACE_DESCRIPTOR *pIntfDesc = 0;
	uint32_t next_desc_adr;

	pD = (USB_COMMON_DESCRIPTOR *) pDesc;
	next_desc_adr = (uint32_t) pDesc;

	while (pD->bLength) {
		/* is it interface descriptor */
		if (pD->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE) {

			pIntfDesc = (USB_INTERFACE_DESCRIPTOR *) pD;
			/* did we find the right interface descriptor */
			if (pIntfDesc->bInterfaceClass == intfClass) {
				break;
			}
		}
		pIntfDesc = 0;
		next_desc_adr = (uint32_t) pD + pD->bLength;
		pD = (USB_COMMON_DESCRIPTOR *) next_desc_adr;
	}

	return pIntfDesc;
}

inline uint32_t skynet_cdc_read(uint8_t *pBuf, uint32_t buf_len) {
	return vcom_bread(pBuf, buf_len);
	//return vcom_read_req(pBuf, buf_len);
}

inline uint32_t skynet_cdc_write(uint8_t *pBuf, uint32_t len) {
	return vcom_write(pBuf, len);
}

unsigned char usb_cdc_write_buf[USB_MAX_MESSAGE_LENGTH];
unsigned int usb_cdc_write_pos = 0;

extern VCOM_DATA_T g_vCOM;

inline uint32_t skynet_cdc_write_buffered(uint8_t *pBuf, uint32_t len) {
	return RingBuffer_InsertMult(&usb_tx_ringbuf, pBuf, len);
}

uint32_t skynet_cdc_write_message(usb_message *msg) {
	msg->magic = USB_MAGIC_NUMBER;
	if (msg->seqno == 0) msg->seqno = current_seqno++;

	uint16_t cnt = 0;
	cnt += skynet_cdc_write_buffered((unsigned char*)msg, USB_HEADER_SIZE);
	cnt += skynet_cdc_write_buffered((unsigned char*)msg->payload, msg->payload_length);

	return cnt;
}

uint32_t skynet_cdc_write_debug(const char* format, ... ) {
	char buf[USB_MAX_PAYLOAD_LENGTH - USB_HEADER_SIZE];

	va_list args;
	va_start(args, format);
	int n = vsnprintf(buf, (int)USB_MAX_PAYLOAD_LENGTH, format, args);
	va_end(args);

	if (n < 0) return n; // error case

	usb_message msg;
	memset(&msg, 0, sizeof(usb_message));
	msg.type = USB_DEBUG;
	msg.payload_length = n;
	msg.payload = buf;
	return skynet_cdc_write_message(&msg);
}


char usb_payload_buf[USB_MAX_PAYLOAD_LENGTH];
int usb_read_pos = 0;
bool usb_reverse = false;
usb_message usb_received_message;
usb_receive_state usb_rx_state = USB_RECEIVE_IDLE;

void skynet_cdc_receive_data(void) {
	usb_received_message.payload = usb_payload_buf;

	unsigned char b[64];
	int c = skynet_cdc_read(b, sizeof(b));
	//int c = RingBuffer_PopMult(&usb_rx_ringbuf, b, sizeof(b));

	/*
#ifdef DEBUG
	if (c > 0) {
		DBG("%d\n", c);
		for (int i = 0; i < c; ++i) {
			DBG("%x ", b[i]);
		}
		DBG("\n");
	}
#endif
	*/

	for (int i = 0; i < c; ++i) {
		unsigned char buf = b[i];

		// expect magic number
		if (usb_rx_state == USB_RECEIVE_IDLE) {
			if (buf == USB_MAGIC_BYTE1) {
				// wrong byte order, reverse
				usb_reverse = true;
				usb_rx_state = USB_RECEIVE_MAGIC;
			}
			else if (buf == USB_MAGIC_BYTE2) {
				// correct byte order, no reverse
				usb_reverse = false;
				usb_rx_state = USB_RECEIVE_MAGIC;
			}
			else {
				usb_rx_state = USB_RECEIVE_IDLE;
			}
		}
		else if (usb_rx_state == USB_RECEIVE_MAGIC) {
			if (usb_reverse == true && buf == USB_MAGIC_BYTE2) {
				usb_rx_state = USB_RECEIVE_TYPE;
			}
			else if (usb_reverse == false && buf == USB_MAGIC_BYTE1) {
				usb_rx_state = USB_RECEIVE_TYPE;
			}
			else {
				usb_rx_state = USB_RECEIVE_IDLE;
			}
		}
		else if (usb_rx_state == USB_RECEIVE_TYPE) {
			usb_received_message.type = buf;
			usb_rx_state = USB_RECEIVE_SEQNO;
			// TODO check type is if valid?
		}
		else if (usb_rx_state == USB_RECEIVE_SEQNO) {
			usb_received_message.seqno = buf;
			usb_rx_state = USB_RECEIVE_PAYLOAD_LENGTH;
			usb_read_pos = 0;
			usb_received_message.payload_length = 0;
		}
		else if (usb_rx_state == USB_RECEIVE_PAYLOAD_LENGTH) {
			// TODO revert if neccessary
			//printf("length: %d %d\n", usb_read_pos, usb_read_pos*8);

			if (usb_reverse) {
				usb_received_message.payload_length += ((int)buf << ((3 - usb_read_pos)*8));
			}
			else {
				usb_received_message.payload_length += ((int)buf << ((usb_read_pos)*8));
			}
			usb_read_pos++;
			if (usb_read_pos == 4) {
				usb_rx_state = USB_RECEIVE_PAYLOAD;
				usb_read_pos = 0;
			}
		}
		else if (usb_rx_state == USB_RECEIVE_PAYLOAD) {
			usb_received_message.payload[usb_read_pos] = buf;
			usb_read_pos++;

			if (usb_read_pos == usb_received_message.payload_length) {
				usb_rx_state = USB_RECEIVE_IDLE;

				// copy msg
				usb_message *new_msg = malloc(sizeof(usb_message));												malloc_count();
				char *new_msg_payload = malloc(usb_received_message.payload_length * sizeof(unsigned char));	malloc_count();

				if (new_msg == NULL || new_msg_payload == NULL) {
					DBG("Not enough heap memory for buffering USB packet. Aborting.\n");
					break;
				}

				memcpy(new_msg, &usb_received_message, sizeof(usb_message));
				memcpy(new_msg_payload, usb_received_message.payload, usb_received_message.payload_length * sizeof(unsigned char));
				new_msg->payload = new_msg_payload;

				// Now enqueue event. On error free space again.
				if (!events_enqueue(EVENT_USB_RX_MESSAGE, new_msg)) {
					free(new_msg_payload);	free_count();
					free(new_msg);			free_count();
					new_msg_payload = NULL;
					new_msg = NULL;
				}
				//usb_message_avail = true;
				//printf("event\n");
			}
		}
	}
}

extern VCOM_DATA_T g_vCOM;

void skynet_cdc_task(void) {
	// send
	VCOM_DATA_T *pVcom = &g_vCOM;
	if ((pVcom->tx_flags & VCOM_TX_CONNECTED) && ((pVcom->tx_flags & VCOM_TX_BUSY) == 0) && (!RingBuffer_IsEmpty(&usb_tx_ringbuf))) {
		uint8_t buf[USB_MAX_PACKET0];
		uint16_t cnt = RingBuffer_PopMult(&usb_tx_ringbuf, buf, USB_MAX_PACKET0);
		vcom_write(buf, cnt);
	}

	// reschedule
	register_delayed_event(1, skynet_cdc_task);
}

void skynet_cdc_flush_buffers(void) {
	RingBuffer_Flush(&usb_tx_ringbuf);
}
