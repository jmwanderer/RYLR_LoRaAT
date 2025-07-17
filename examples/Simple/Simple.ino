// Copyright (c) James Wanderer. All rights reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Example runs on an ESP32 S2
// RYLR998 TX and RX connected to pins 3 and 5 on ESP32 S2

#include <Arduino.h>
#include "rylr_loraat.h"

RYLR_LoRaAT rylr;
uint16_t local_address = 1;

void setup() {
  Serial.begin(115200);
  delay(1000); // Let serial port start

  // Connect to LoRa device
  // Using pins 3 and 5 on an esp32 s2. Change for different platform.
  Serial1.begin(115200, SERIAL_8N1, 3, 5);
  rylr.setSerial(&Serial1);

  int result = rylr.checkStatus();
  Serial.print("check status result: ");
  Serial.println(result);

  // Set parameters of LoRa device.
  rylr.setAddress(local_address);
  result = rylr.setRFPower(14);
}

// Time last hello sent.
unsigned long last_hello = 0;

void loop() {
  RYLR_LoRaAT_Message *message;

  unsigned long now = millis();
  if (now - last_hello > 20000) {
    // Broadcast hello every 20 seconds.
    last_hello = now;
    Serial.println("TX HELLO message");
    rylr.startTxMessage();
    rylr.addTxData("HELLO");
    rylr.sendTxMessage(0);
  }

  // Handle received messages
  if ((message = rylr.checkMessage())) {
    Serial.print("RX message from: ");
    Serial.print(message->from_address);
    Serial.print(", rssi: ");
    Serial.print(message->rssi);
    Serial.print(", snr: ");
    Serial.print(message->snr);
    Serial.print(", data length: ");
    Serial.print(message->data_len);
    Serial.print(", data: ");
    Serial.print(message->data);
    Serial.println();
 }
}
