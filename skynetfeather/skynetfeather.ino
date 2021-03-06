#include <SPI.h>
#include <RH_RF95.h>

#include "skynetfeathermac/skymac.h"
#include "skynetfeathermac/usb_message.h"
#include "skynetfeathermac/config.h"
#include "skynetfeathermac/rf_packet_handler.h"

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define LED 13
#define RF95_FREQ 868.3

RH_RF95 rf95(RFM95_CS, RFM95_INT);
skynetfeatherconfig cfg;

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

char buf[128];

void setup() {
  // init LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);

  // manual reset of radio module
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  // init Serial
  //while (!Serial);    // DEBUG: wait for serial monitor to connect
  Serial.begin(9600);

  // init radio module
  while (!rf95.init()) {
    while (1);
  }
  if (!rf95.setFrequency(RF95_FREQ)) {
    while (1);
  }
  //  rf95.setTxPower(23, false);

  // configure radio module (datarate, spreading, ...)
  RH_RF95::ModemConfig modemCfg;
  modemCfg.reg_1d = (0b1001 << 4) | (0b001 << 1); // 500kHz, 4/5
  modemCfg.reg_1e = (1 << 2) | (0x9 << 4); // CRC on, 512 chips/symbol
  modemCfg.reg_26 = 0;
  rf95.setModemRegisters(&modemCfg);


  // read out serial number of CPU and write to config
  cfg.serial0 = *((uint32_t*)0x0080A00C);
  cfg.serial1 = *((uint32_t*)0x0080A040);
  cfg.serial2 = *((uint32_t*)0x0080A044);
  cfg.serial3 = *((uint32_t*)0x0080A048);
  cfg.mac0 = (cfg.serial0 >> 8) & 0xFF;
  cfg.mac1 = (cfg.serial0     ) & 0xFF;
}

unsigned long last_pkt = 0;

void loop() {

  
    if (rf95.available()) {
      // Should be a message for us now
      uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
      uint8_t len = sizeof(buf);
  
      if (rf95.recv(buf, &len)) {
        digitalWrite(LED, HIGH);

        rf_handler(buf, len);

        /*
        RH_RF95::printBuffer("Received: ", buf, len);
        
        Serial.print("Got: ");
        Serial.print(len, DEC);
        //Serial.println((char*)buf);
        Serial.print(", RSSI: ");
        Serial.println(rf95.lastRssi(), DEC);
        */
  
        //Serial.println((char*)(inframe.payload));

/*
        usb_message m;
        memset(&m, 0, sizeof(usb_message));
        m.magic = 43981;
        m.type = USB_SKYNET_PACKET;
        m.seqno = 0;
        m.payload_length = len;
        m.payload = (char*)buf;


        Serial.write((char*)&m, USB_HEADER_SIZE);
        Serial.write((char*)(m.payload), m.payload_length);
*/
  
        //delay(5); // minimum
  
        digitalWrite(LED, LOW);
      }
      else
      {
        //Serial.println("Receive failed");
      }
    }
  
}

#define MAC_0   (0x5D)
#define MAC_1   (0x5D)



