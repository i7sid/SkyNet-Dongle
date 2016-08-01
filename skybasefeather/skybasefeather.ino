#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define LED 13
#define RF95_FREQ 867.0

//RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);


  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  /*
    digitalWrite(RFM95_RST, HIGH);
    delay(10);
  */

  //while (!Serial);
  Serial.begin(9600);

  /*
    while (!rf95.init()) {
    while (1);
    }

    // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
    if (!rf95.setFrequency(RF95_FREQ)) {
    while (1);
    }
    //rf95.setTxPower(23, false);
  */

  Serial1.begin(9600);

  Serial.write("Init complete.\n");
}
//unsigned char i = 0;
void loop() {
  while (Serial1.available()) {
    digitalWrite(LED, HIGH);
    char c = Serial1.read();
    Serial.print("char: ");
    Serial.print(c, DEC);/*
    Serial.print(" (");
    Serial.print(c);
    Serial.print(" )");*/
    Serial.print("\n");
    //Serial1.write(i);
    //i++;
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

