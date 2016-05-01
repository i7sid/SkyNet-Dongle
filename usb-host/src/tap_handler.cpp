/**
 *  @file
 *
 *  @date 	01.05.2016
 *  @author	Michael Zapf <michael.zapf@fau.de>
 */


#include <fstream>
#include <iomanip>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <arpa/inet.h>

#include <linux/ip.h>

#include <usb/message.h>
#include <mac/mac.h>
#include "error_handler.h"
#include "tap.h"
#include "usbtty.h"
#include "gui/gui.h"

using namespace std;

extern tap* ptr_tap;
extern usb_tty* ptr_tty;


void tapReceiveHandler(void *pkt, size_t nread) {
	char *p = (char*)pkt;
	struct ether_header* frame = (struct ether_header*)(p);

	size_t data_len = nread - sizeof(struct ether_header);
	if (USB_MAX_PAYLOAD_LENGTH < data_len) {
		COLOR_ERR();
		cerr << "Packet received via network too large for usb tty. Ignoring." << endl;
		COLOR_RESET();
		return;
	}

	// construct real skynet-mac-packet
	mac_frame_data mac_frame;
	mac_frame_data_init(&mac_frame);
	mac_frame.payload = (uint8_t*)(p + sizeof(struct ether_header));
	mac_frame.payload_size = data_len;

	MHR_FC_SET_DEST_ADDR_MODE(mac_frame.mhr.frame_control, MAC_ADDR_MODE_LONG);
	mac_frame.mhr.dest_pan_id[0] = 0;
	mac_frame.mhr.dest_pan_id[1] = 0;
	mac_frame.mhr.dest_address[0] = frame->ether_dhost[0];
	mac_frame.mhr.dest_address[1] = frame->ether_dhost[1];
	mac_frame.mhr.dest_address[2] = frame->ether_dhost[2];
	mac_frame.mhr.dest_address[3] = frame->ether_dhost[3];
	mac_frame.mhr.dest_address[4] = frame->ether_dhost[4];
	mac_frame.mhr.dest_address[5] = frame->ether_dhost[5];
	mac_frame.mhr.dest_address[6] = 0;
	mac_frame.mhr.dest_address[7] = 0;

	MHR_FC_SET_SRC_ADDR_MODE(mac_frame.mhr.frame_control, MAC_ADDR_MODE_LONG);
	mac_frame.mhr.src_pan_id[0] = 0;
	mac_frame.mhr.src_pan_id[1] = 0;
	mac_frame.mhr.src_address[0] = frame->ether_shost[0];
	mac_frame.mhr.src_address[1] = frame->ether_shost[1];
	mac_frame.mhr.src_address[2] = frame->ether_shost[2];
	mac_frame.mhr.src_address[3] = frame->ether_shost[3];
	mac_frame.mhr.src_address[4] = frame->ether_shost[4];
	mac_frame.mhr.src_address[5] = frame->ether_shost[5];
	mac_frame.mhr.src_address[6] = 0;
	mac_frame.mhr.src_address[7] = 0;


	// generate extended headers
	mac_extheader hdr;
	mac_extheader_init(&hdr);
	hdr.typelength_union.type_length.length = 2;
	hdr.typelength_union.type_length.type = EXTHDR_ETHER_TYPE;
	hdr.data[0] = ((frame->ether_type & 0xFF00) >> 8);
	hdr.data[1] = ((frame->ether_type & 0x00FF)     );

	mac_frame.extheader = &hdr;

	uint8_t *payload = new uint8_t[4096];
    memset(payload, 0, 4096);
	int mac_cnt = mac_frame_data_pack(&mac_frame, payload);
	mac_frame_calc_crc(payload, mac_cnt);

	usb_message m;
	m.type = USB_SKYNET_PACKET;
	m.payload_length = mac_cnt + PKT_DBG_OVERHEAD;
	m.payload = (char*)payload;


	ptr_tty->usbSendMessage(m);
}


