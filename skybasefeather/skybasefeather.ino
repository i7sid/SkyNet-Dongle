//#include <SPI.h>
#include <RH_RF95.h>

#include "skynetfeathermac/skymac.h"
#include "skynetfeathermac/wdt.h"
#include "skynetfeathermac/config.h"


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

char buf_time[8];
char buf_pos[32];
char buf_wind_dir[16];
char buf_compass[16];
char buf_wind_speed[16];
char buf_hist_wind_speed_short[16];
char buf_hist_wind_dir_short[16];
char buf_hist_wind_speed_long[16];
char buf_hist_wind_dir_long[16];
char buf_hist_wind_speed_diff[16];
char buf_hist_wind_dir_diff[16];
char buf_checksum[8];

char* buf_tokens[12];

char buf[256];

void setup() {
  // init some pointers
  buf_tokens[0]  = buf_time;
  buf_tokens[1]  = buf_pos;
  buf_tokens[2]  = buf_compass;
  buf_tokens[3]  = buf_wind_dir;
  buf_tokens[4]  = buf_wind_speed;
  buf_tokens[5]  = buf_hist_wind_speed_short;
  buf_tokens[6]  = buf_hist_wind_dir_short;
  buf_tokens[7]  = buf_hist_wind_speed_long;
  buf_tokens[8]  = buf_hist_wind_dir_long;
  buf_tokens[9]  = buf_hist_wind_speed_diff;
  buf_tokens[10] = buf_hist_wind_dir_diff;
  buf_tokens[11] = buf_checksum;

  // init LED
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);


  pinMode(RFM95_RST, OUTPUT);

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);


  //while (!Serial);    // DEBUG: wait for serial terminal to be connected
  Serial.begin(9600);


  // rf95 initialization
  while (!rf95.init()) {
    while (1);
  }
  if (!rf95.setFrequency(RF95_FREQ)) {
    while (1);
  }

  // configure radio modem
  rf95.setTxPower(23, false);
  RH_RF95::ModemConfig modemCfg;
  modemCfg.reg_1d = (0b1001 << 4) | (0b001 << 1); // 500kHz, 4/5
  modemCfg.reg_1e = (1 << 2) | (0x9 << 4); // CRC on, 512 chips/symbol
  modemCfg.reg_26 = 0;
  rf95.setModemRegisters(&modemCfg);

  // Init serial communication from LPC1769 (base station)
  Serial1.begin(9600);

  // read out serial number of CPU and write to config
  cfg.serial0 = *((uint32_t*)0x0080A00C);
  cfg.serial1 = *((uint32_t*)0x0080A040);
  cfg.serial2 = *((uint32_t*)0x0080A044);
  cfg.serial3 = *((uint32_t*)0x0080A048);
  cfg.mac0 = (cfg.serial0 >> 8) & 0xFF;
  cfg.mac1 = (cfg.serial0     ) & 0xFF;


  Serial.write("Init complete.\n");
  WDT_init();
  WDT_enable();
}

unsigned long last_pkt = 0;

void loop() {
  WDT_clear();

  // TODO await packets from radio, check and resend if needed (basic multihop)

  // check for data from base station / LPC1769
  while (Serial1.available()) {
    WDT_clear();
    
    digitalWrite(LED, HIGH);
    uint8_t c = Serial1.read();

    buf[cnt++] = c;
    if (c == 0) {
      Serial.write("Received line: ");
      Serial.write(buf, cnt);
      Serial.write("\n");

      // calc checksum
      uint8_t expchecksum = dfx_checksum_calc(buf, cnt - 3);
      
      // check checksum
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

      cnt = 0; // received characters / current position in buffer = 0

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
  WDT_clear();
}

void skynet_send_frame(void) {
  uint8_t buf[256];
  uint16_t pos = 0;

/*
  pos += snprintf((char*)buf, sizeof(buf) - pos,
                  "%s|%s|%s|%s|%s|%s|%s",
                  buf_time, buf_pos, buf_compass, buf_wind_dir, buf_wind_speed,
                  buf_hist_wind_speed_diff, buf_hist_wind_dir_diff);
*/

  pos += snprintf((char*)buf, sizeof(buf) - pos,
                  "%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|",
                  buf_time, buf_pos, buf_compass, buf_wind_dir, buf_wind_speed,
                  buf_hist_wind_speed_short, buf_hist_wind_speed_short,
                  buf_hist_wind_speed_long, buf_hist_wind_dir_long,
                  buf_hist_wind_speed_diff, buf_hist_wind_dir_diff);
  pos++; // terminating null byte
  //Serial.println(pos, DEC);

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

  frame.mhr.src_address[0] = cfg.mac0;
  frame.mhr.src_address[1] = cfg.mac1;

  // ext headers
  mac_extheader hdr;
  mac_extheader_init(&hdr);
  hdr.typelength_union.type_length.type = EXTHDR_SENSOR_VALUES;
//  hdr.typelength_union.type_length.length = 8;
//  hdr.data[0] = SENSOR_POSITION;
//  hdr.data[1] = SENSOR_COMPASS;
//  hdr.data[2] = SENSOR_WIND_DIR;
//  hdr.data[3] = SENSOR_WIND_SPEED;
//  hdr.data[4] = SENSOR_HIST_WIND_SPEED_SHORT;
//  hdr.data[5] = SENSOR_HIST_WIND_DIR_SHORT;
//  hdr.data[6] = SENSOR_HIST_WIND_SPEED_LONG;
//  hdr.data[7] = SENSOR_HIST_WIND_DIR_LONG;
  hdr.typelength_union.type_length.length = 10;
  hdr.data[0] = SENSOR_POSITION;
  hdr.data[1] = SENSOR_COMPASS;
  hdr.data[2] = SENSOR_WIND_DIR;
  hdr.data[3] = SENSOR_WIND_SPEED;
  hdr.data[4] = SENSOR_HIST_WIND_SPEED_SHORT;
  hdr.data[5] = SENSOR_HIST_WIND_DIR_SHORT;
  hdr.data[6] = SENSOR_HIST_WIND_SPEED_LONG;
  hdr.data[7] = SENSOR_HIST_WIND_DIR_LONG;
  hdr.data[8] = SENSOR_HIST_WIND_SPEED_DIFF;
  hdr.data[9] = SENSOR_HIST_WIND_DIR_DIFF;

  mac_extheader hdr2;
  mac_extheader_init(&hdr2);
  hdr2.typelength_union.type_length.type = EXTHDR_TTL;
  hdr2.typelength_union.type_length.length = 1;
  hdr2.data[0] = 2;

  hdr.next = &hdr2;
  frame.extheader = &hdr;

  mac_transmit_packet(&frame, true);
}


