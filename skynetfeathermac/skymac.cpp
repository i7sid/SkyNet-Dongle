#include <Arduino.h>
#include <SPI.h>
#include <RH_RF95.h>
#include "skymac.h"
#include "wdt.h"

#define MAC_SEND_TRIES    (2)
#define RSSI_THRESH 50


static uint8_t seq_no = 0; ///<@brief Current sequence number
extern RH_RF95 rf95;

uint8_t nb; ///<@brief How many backoff-times have been needed in current try?
uint8_t be; ///<@brief Backoff exponent, how many backoff periods shall be waited

int getRssi() {
  uint8_t r = rf95.spiRead(0x1B);
  return r;
}

bool channelFree() {
  rf95.available();
  int rssi = getRssi();
  //Serial.println(rssi, DEC);
  return (rssi <= RSSI_THRESH);
}

/// @brief  Returns a pseudo random number (strictly) smaller than max.
inline int mac_random(int max) {
    return (rand() % max);
}

void mac_init(void) {
  seq_no = (uint8_t)mac_random(0x100);
}

bool mac_transmit_packet(mac_frame_data *frame, bool new_seq) {
    uint16_t est_size = mac_frame_data_estimate_size(frame);
    uint8_t buf[est_size];
    if (new_seq) { frame->mhr.seq_no = seq_no++; }
    memset(buf, 0, sizeof(buf));                // TODO remove, DEBUG!
    uint16_t size = mac_frame_data_pack(frame, buf);
    //mac_frame_calc_crc(buf, size); // TODO
    
    bool r = false;
    for (uint8_t i = 0; i < MAC_SEND_TRIES; ++i) {
        r |= mac_transmit_data(buf, size);
        delay(15);
    }
    if (!r) return false;

    /*
       uint16_t size = mac_frame_data_get_size(frame);
       uint8_t buf[size];
       mac_frame_data_pack(frame, buf);
       mac_frame_calc_crc(buf, sizeof(buf));
       return mac_transmit_data(buf, size);
       */
}

bool mac_transmit_data(uint8_t* data, uint16_t l) {
     
    nb = 0;
    be = MAC_CONST_MIN_BE;

    do {
      // initial delay
      int del = mac_random((1 << be) - 1);  // is: 2^be - 1
      delay(del * MAC_CONST_A_UNIT_BACKOFF_PERIOD * MAC_CONST_SYMBOL_LENGTH);
      //DBG("mac slept\n");
      WDT_clear();


      if (channelFree()) {
        rf95.send(data, l);
        delay(1);
        rf95.waitPacketSent();
        return true;
      }
      else {
        nb++;
        be = min(be + 1, MAC_CONST_MAX_BE);
      }
    } while (nb <= MAC_CONST_MAX_CSMA_BACKOFFS);
    return false;

    /*
  rf95.send(data, l);
  delay(1);
  rf95.waitPacketSent();
  return true;
  */
}


//#ifdef __cplusplus
//extern "C" {
//#endif


void mac_frame_data_free_contents(mac_frame_data *frame) {
    if (frame->payload != NULL) { free(frame->payload); free_count(); }
    mac_frame_extheaders_free(frame->extheader); // nothing happens if NULL
    //free(frame); // do not do this!
}


/*
   size_t mac_frame_data_get_size(mac_frame_data *frame) {
// TODO anpassen, siehe unten (mac_frame_data_pack())
return sizeof(frame->mhr) + frame->payload_size + sizeof(frame->fcs);
}
*/
size_t mac_frame_data_estimate_size(mac_frame_data *frame) {
    // maximum of header fields as defined in max_frame_data + payload - field "payload_size"
    return sizeof(mac_frame_data) + frame->payload_size - sizeof(uint16_t);
}

void mac_frame_data_init(mac_frame_data *frame) {
    memset(frame, 0, sizeof(mac_frame_data));
    MHR_FC_SET_FRAME_TYPE(frame->mhr.frame_control, MAC_FRAME_DATA);
    MHR_FC_SET_FRAME_VERSION(frame->mhr.frame_control, 0x01);

    // TODO set new seq_no
}

//#include <stdio.h>    // DEBUG!
uint16_t mac_frame_data_pack(mac_frame_data *frame, uint8_t *buffer) {
    uint16_t pos = 0;
    // copy MHR
    switch ((mac_frame_types)(MHR_FC_GET_FRAME_TYPE(frame->mhr.frame_control))) {
        case MAC_FRAME_BEACON:
            break;
        case MAC_FRAME_DATA:
            {
                // ignore security for first implementation
                MHR_FC_SET_SECURITY_ENABLED(frame->mhr.frame_control, 0);

                // ignore frame pending for first implementation
                MHR_FC_SET_FRAME_PENDING(frame->mhr.frame_control, 0);

                // ignore ack request for first implementation
                MHR_FC_SET_ACK_REQUEST(frame->mhr.frame_control, 0);


                // copy frame control
                buffer[pos++] = frame->mhr.frame_control[0];
                buffer[pos++] = frame->mhr.frame_control[1];

                // copy seqno
                buffer[pos++] = frame->mhr.seq_no;

                // copy dest pan id (if it shall be present)
                if (MHR_FC_GET_DEST_ADDR_MODE(frame->mhr.frame_control) != MAC_ADDR_MODE_NOT) {
                    buffer[pos++] = frame->mhr.dest_pan_id[0]; // TODO endianness?
                    buffer[pos++] = frame->mhr.dest_pan_id[1];
                }

                // copy dest address
                if (MHR_FC_GET_DEST_ADDR_MODE(frame->mhr.frame_control) == MAC_ADDR_MODE_SHORT) {
                    // copy 2 bytes (short address)
                    //pos += 2;
                    buffer[pos++] = frame->mhr.dest_address[0]; // TODO endianness?
                    buffer[pos++] = frame->mhr.dest_address[1];
                }
                else if (MHR_FC_GET_DEST_ADDR_MODE(frame->mhr.frame_control) == MAC_ADDR_MODE_LONG) {
                    // copy 8 bytes (long address)
                    buffer[pos++] = frame->mhr.dest_address[0]; // TODO endianness?
                    buffer[pos++] = frame->mhr.dest_address[1];
                    buffer[pos++] = frame->mhr.dest_address[2];
                    buffer[pos++] = frame->mhr.dest_address[3];
                    buffer[pos++] = frame->mhr.dest_address[4];
                    buffer[pos++] = frame->mhr.dest_address[5];
                    buffer[pos++] = frame->mhr.dest_address[6];
                    buffer[pos++] = frame->mhr.dest_address[7];
                }

                if (!(MHR_FC_GET_PAN_ID_COMPRESSION(frame->mhr.frame_control)) &&
                        (MHR_FC_GET_SRC_ADDR_MODE(frame->mhr.frame_control) != MAC_ADDR_MODE_NOT)) {
                    buffer[pos++] = frame->mhr.src_pan_id[0]; // TODO endianess?
                    buffer[pos++] = frame->mhr.src_pan_id[1];
                }

                // copy src address
                if (MHR_FC_GET_SRC_ADDR_MODE(frame->mhr.frame_control) == MAC_ADDR_MODE_SHORT) {
                    // copy 2 bytes (short address)
                    buffer[pos++] = frame->mhr.src_address[0]; // TODO endianness?
                    buffer[pos++] = frame->mhr.src_address[1];
                }
                else if (MHR_FC_GET_SRC_ADDR_MODE(frame->mhr.frame_control) == MAC_ADDR_MODE_LONG) {
                    // copy 8 bytes (long address)
                    buffer[pos++] = frame->mhr.src_address[0]; // TODO endianness?
                    buffer[pos++] = frame->mhr.src_address[1];
                    buffer[pos++] = frame->mhr.src_address[2];
                    buffer[pos++] = frame->mhr.src_address[3];
                    buffer[pos++] = frame->mhr.src_address[4];
                    buffer[pos++] = frame->mhr.src_address[5];
                    buffer[pos++] = frame->mhr.src_address[6];
                    buffer[pos++] = frame->mhr.src_address[7];
                }

                // ignore aux security header for now

                break;
            }
        case MAC_FRAME_ACK:
            break;
        case MAC_FRAME_MAC_CMD:
            break;
    }

    // copy extheaders
    mac_extheader* hdr = frame->extheader;

    while (hdr != NULL) {
        buffer[pos++] = hdr->typelength_union.raw;
        for (uint8_t i = 0; i < hdr->typelength_union.type_length.length; ++i) {
            buffer[pos++] = hdr->data[i];
        }
        hdr = hdr->next;
    }
    buffer[pos++] = 0x0; // last extheader

    // copy payload
    memcpy(buffer + pos, frame->payload, frame->payload_size);
    pos += frame->payload_size;

    // copy empty fcs
    buffer[pos++] = 0;
    buffer[pos++] = 0;

    return pos;
}


uint16_t mac_frame_data_unpack(mac_frame_data *frame, uint8_t *buffer, uint16_t length) {
    // copy frame control
    uint16_t pos = 0;
    frame->mhr.frame_control[0] = buffer[pos++];
    frame->mhr.frame_control[1] = buffer[pos++];

    // copy seqno
    frame->mhr.seq_no = buffer[pos++];


    // copy dest pan id (if it shall be present)
    if (MHR_FC_GET_DEST_ADDR_MODE(frame->mhr.frame_control) != MAC_ADDR_MODE_NOT) {
        frame->mhr.dest_pan_id[0] = buffer[pos++]; // TODO endianness?
        frame->mhr.dest_pan_id[1] = buffer[pos++];
    }

    // copy dest address
    if (MHR_FC_GET_DEST_ADDR_MODE(frame->mhr.frame_control) == MAC_ADDR_MODE_SHORT) {
        // copy 2 bytes (short address)
        frame->mhr.dest_address[0] = buffer[pos++]; // TODO endianness?
        frame->mhr.dest_address[1] = buffer[pos++];
    }
    else if (MHR_FC_GET_DEST_ADDR_MODE(frame->mhr.frame_control) == MAC_ADDR_MODE_LONG) {
        // copy 8 bytes (long address)
        frame->mhr.dest_address[0] = buffer[pos++]; // TODO endianness?
        frame->mhr.dest_address[1] = buffer[pos++];
        frame->mhr.dest_address[2] = buffer[pos++];
        frame->mhr.dest_address[3] = buffer[pos++];
        frame->mhr.dest_address[4] = buffer[pos++];
        frame->mhr.dest_address[5] = buffer[pos++];
        frame->mhr.dest_address[6] = buffer[pos++];
        frame->mhr.dest_address[7] = buffer[pos++];
    }

    if (!(MHR_FC_GET_PAN_ID_COMPRESSION(frame->mhr.frame_control)) &&
            (MHR_FC_GET_SRC_ADDR_MODE(frame->mhr.frame_control) != MAC_ADDR_MODE_NOT)) {
        frame->mhr.src_pan_id[0] = buffer[pos++]; // TODO endianess?
        frame->mhr.src_pan_id[1] = buffer[pos++];
    }

    // copy src address
    if (MHR_FC_GET_SRC_ADDR_MODE(frame->mhr.frame_control) == MAC_ADDR_MODE_SHORT) {
        // copy 2 bytes (short address)
        frame->mhr.src_address[0] = buffer[pos++]; // TODO endianness?
        frame->mhr.src_address[1] = buffer[pos++];
    }
    else if (MHR_FC_GET_SRC_ADDR_MODE(frame->mhr.frame_control) == MAC_ADDR_MODE_LONG) {
        // copy 8 bytes (long address)
        frame->mhr.src_address[0] = buffer[pos++]; // TODO endianness?
        frame->mhr.src_address[1] = buffer[pos++];
        frame->mhr.src_address[2] = buffer[pos++];
        frame->mhr.src_address[3] = buffer[pos++];
        frame->mhr.src_address[4] = buffer[pos++];
        frame->mhr.src_address[5] = buffer[pos++];
        frame->mhr.src_address[6] = buffer[pos++];
        frame->mhr.src_address[7] = buffer[pos++];
    }

    // extheaders
    union typelength_union next_ext_type;
    next_ext_type.raw = buffer[pos++];
    mac_extheader *last_header = NULL;
    while (next_ext_type.type_length.type != EXTHDR_NO) {
        mac_extheader* hdr = (mac_extheader*)malloc(sizeof(mac_extheader)); malloc_count();
        if (hdr == NULL) { return pos; /* TODO throw error */ }

        // clear memory and fill/copy data
        mac_extheader_init(hdr);
        hdr->typelength_union.raw = next_ext_type.raw;
        memcpy(hdr->data, &(buffer[pos]), next_ext_type.type_length.length);
        
        // skip already copied bytes of input
        pos += next_ext_type.type_length.length;

        // make sure all headers are accessable (chaining)
        if (last_header == NULL) {
            frame->extheader = hdr;
            last_header = hdr;
        }
        else {
            last_header->next = hdr;
            last_header = hdr;
        }

        next_ext_type.raw = buffer[pos++]; // goto next extheader in input
    }


    // payload
    frame->payload_size = length - 2 - pos;
    frame->payload = (uint8_t*)malloc(frame->payload_size+1);   malloc_count(); // +1 for additional NULL byte
    if (frame->payload == NULL) {
        // TODO throw error? return -1?
        return pos;
    }

    /*
       DBG("d: %d\n", length);
       DBG("e: %d\n", frame->payload_size);
       */
    memcpy(frame->payload, buffer + pos, frame->payload_size);

    // additional function for more comfort: add trailing null byte
    frame->payload[frame->payload_size] = 0;

    pos += frame->payload_size;

    // fcs
    frame->fcs[0] = buffer[pos++];
    frame->fcs[1] = buffer[pos++];

    return pos;
}
// * /

void mac_frame_extheaders_free(mac_extheader* hdr) {
    if (hdr == NULL) return;
    mac_frame_extheaders_free(hdr->next);
    free(hdr);    free_count();
}

//#ifdef __cplusplus
//}
//#endif
