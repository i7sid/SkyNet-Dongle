#include <SPI.h>
#include <RH_RF95.h>

#include "skymac.h"
#include "usb_message.h"

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
  //rf95.setTxPower(23, false);


  //Serial1.begin(9600);

  //Serial.write("Init complete.\n");
}

unsigned long last_pkt = 0;

void loop() {

  
    if (rf95.available()) {
      // Should be a message for us now
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);
  
      if (rf95.recv(buf, &len)) {
        digitalWrite(LED, HIGH);

        /*
        RH_RF95::printBuffer("Received: ", buf, len);
        
        Serial.print("Got: ");
        Serial.print(len, DEC);
        //Serial.println((char*)buf);
        Serial.print(", RSSI: ");
        Serial.println(rf95.lastRssi(), DEC);
        */

        usb_message m;
        memset(&m, 0, sizeof(usb_message));
        m.magic = 43981;
        m.type = USB_SKYNET_PACKET;
        m.seqno = 0;
        m.payload_length = len;
        m.payload = (char*)buf;

        Serial.write((char*)&m, USB_HEADER_SIZE);
        Serial.write((char*)(m.payload), m.payload_length);


  /*
        mac_frame_data inframe;
        mac_frame_data_init(&inframe);

      
        mac_frame_data_unpack(&inframe, buf, (uint16_t)len);


        Serial.println((char*)(inframe.payload));
  */
        //delay(5); // minimum
  
        digitalWrite(LED, LOW);
      }
      else
      {
        Serial.println("Receive failed");
      }
    }
  
}

#define MAC_0   (0x5D)
#define MAC_1   (0x5D)



