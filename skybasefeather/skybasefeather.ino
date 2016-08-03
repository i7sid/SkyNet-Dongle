#include <SPI.h>
#include <RH_RF95.h>

#include "skymac.h"

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define LED 13
#define RF95_FREQ 867.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

inline uint8_t dfx_checksum_calc(char* data, uint16_t length) {
  uint8_t sum = 0;
  char* pos = data;
  uint32_t n = length;

  while (n-- > 0) {
    sum += *(pos++);
  }

  return (~sum);
}

uint16_t cnt = 0;

char buf_time[8];
char buf_pos[32];
char buf_wind_dir[16];
char buf_compass[16];
char buf_wind_speed[16];
char buf_checksum[8];

char* buf_tokens[6];

char buf[128];

void setup() {
  buf_tokens[0] = buf_time;
  buf_tokens[1] = buf_pos;
  buf_tokens[2] = buf_compass;
  buf_tokens[3] = buf_wind_dir;
  buf_tokens[4] = buf_wind_speed;
  buf_tokens[5] = buf_checksum;

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);


  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);


  //while (!Serial);
  Serial.begin(9600);


  while (!rf95.init()) {
    while (1);
  }

  if (!rf95.setFrequency(RF95_FREQ)) {
    while (1);
  }
  rf95.setTxPower(23, false);


  Serial1.begin(9600);

  Serial.write("Init complete.\n");
}

unsigned long last_pkt = 0;

void loop() {
  while (Serial1.available()) {
    digitalWrite(LED, HIGH);
    uint8_t c = Serial1.read();

    buf[cnt++] = c;
    if (c == 0) {
      Serial.write("Received line: ");
      Serial.write(buf, cnt);
      Serial.write("\n");

      // calc checksum
      uint8_t expchecksum = dfx_checksum_calc(buf, cnt - 3);
      
      // TODO: check checksum
      char* cchecksum = buf + cnt - 3;
      uint8_t ichecksum = (uint8_t)(strtoul(cchecksum, NULL, 16));
      if (expchecksum != ichecksum) {
        // ERROR: wrong checksum, abort
        continue;
      }

      // now tokenize
      char delimiter[] = "|";
      char *ptr;
      uint8_t i = 0;
      ptr = strtok(buf, delimiter);

      while (ptr != NULL && i < sizeof(buf_tokens)) {
        strcpy(buf_tokens[i++], ptr);
        ptr = strtok(NULL, delimiter);
      }

/*
      Serial.write(buf_time, strlen(buf_time));
      Serial.write("\n");
      Serial.write(buf_pos, strlen(buf_pos));
      Serial.write("\n");
      Serial.write(buf_compass, strlen(buf_compass));
      Serial.write("\n");
      Serial.write(buf_wind_dir, strlen(buf_wind_dir));
      Serial.write("\n");
      Serial.write(buf_wind_speed, strlen(buf_wind_speed));
      Serial.write("\n");
      */

      cnt = 0;

      unsigned long n = millis();
      if (n - last_pkt > 5000) {
        skynet_send_frame();
        last_pkt = n;
      }
      
    }
    continue;
  }
  //delay(10);
  digitalWrite(LED, LOW);


  /*
    if (rf95.available()) {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len)) {
      digitalWrite(LED, HIGH);
      /*
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      Serial.println((char*)buf);
       Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
        /

      delay(5); // minimum

      digitalWrite(LED, LOW);
    }
    else
    {
      Serial.println("Receive failed");
    }
    }
  */
}

#define MAC_0   (0x5D)
#define MAC_1   (0x5D)

void skynet_send_frame(void) {
  uint8_t buf[128];
  uint16_t pos = 0;

  pos += snprintf((char*)buf, sizeof(buf) - pos,
                  "%s|%s|%s|%s|%s",
                  buf_time, buf_pos, buf_compass, buf_wind_dir, buf_wind_speed);
  pos++; // terminating null byte

  Serial.println((char*)buf);
  
  mac_frame_data frame;
  mac_frame_data_init(&frame);
  frame.payload = buf;
  frame.payload_size = pos;
  //frame.payload_size = pos+4; // TODO Debug (+4)

  MHR_FC_SET_DEST_ADDR_MODE(frame.mhr.frame_control, MAC_ADDR_MODE_SHORT);
  MHR_FC_SET_SRC_ADDR_MODE(frame.mhr.frame_control, MAC_ADDR_MODE_SHORT);

  // MAC addresses
  //NV_DATA_T *config = skynet_nv_get();
  frame.mhr.dest_address[0] = 0xFF;
  frame.mhr.dest_address[1] = 0xFF;
  //frame.mhr.src_address[0] = config->mac_addr[4];
  //frame.mhr.src_address[1] = config->mac_addr[5];

  frame.mhr.src_address[0] = MAC_0;
  frame.mhr.src_address[1] = MAC_1;

  // ext headers
  mac_extheader hdr;
  mac_extheader_init(&hdr);
  hdr.typelength_union.type_length.type = EXTHDR_SENSOR_VALUES;
  hdr.typelength_union.type_length.length = 4;
  hdr.data[0] = SENSOR_POSITION;
  hdr.data[1] = SENSOR_COMPASS;
  hdr.data[2] = SENSOR_WIND_DIR;
  hdr.data[3] = SENSOR_WIND_SPEED;
  mac_extheader hdr2;
  mac_extheader_init(&hdr2);
  hdr2.typelength_union.type_length.type = EXTHDR_TTL;
  hdr2.typelength_union.type_length.length = 1;
  hdr2.data[0] = 2;

  hdr.next = &hdr2;
  frame.extheader = &hdr;

  mac_transmit_packet(&frame, true);

  // TODO calc checksum
}


