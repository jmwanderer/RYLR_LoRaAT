// Copyright (c) James Wanderer. All rights reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Ping - Pong and Hello between 2 or more Reyax LoRa UART devices.

// Example runs on an ESP32 S2
// RYLR998 TX and RX connected to pins 3 and 5 on ESP32 S2

#include <Arduino.h>
#include <Preferences.h>
#include "RYLR_LoRaAT.h"


#define LED_PIN 15  // Change for platform or use BUILTIN_LED

RYLR_LoRaAT rylr;
Preferences prefs;
bool initialized = false;
uint16_t local_address = 0;

void setup() {
  Serial.begin(115200);
  delay(1000); // Let serial port start

  // Connect to LoRa device
  // Using pins 3 and 5 on an esp32 s2. Change for different platform.
  Serial1.begin(115200, SERIAL_8N1, 3, 5);
  rylr.setSerial(&Serial1);

  // Report status via built-in LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Local address is stored in "config" preferences.
  prefs.begin("config");
  local_address = prefs.getUShort("address");

  // Prompt user over serial port for local address if needed.
  if (local_address == 0) {
    local_address = promptUserForAddress();
    prefs.putUShort("address", local_address);
  }
  Serial.print("Local address: ");
  Serial.println(local_address);

  // Check status of LoRa device.
  int result = rylr.checkStatus();
  Serial.print("check status result: ");
  Serial.println(result);

  // Set parameters of LoRa device.
  bool ok = true;
  result = rylr.setAddress(local_address);
  ok = (result == 0);
  result = rylr.setRFParameters(9, 7, 1, 12);
  ok &= (result == 0);
  result = rylr.setRFPower(14);
  ok &= (result == 0);
  result = rylr.setPassword("CA0043A7");
  ok &= (result == 0);

  initialized = ok;
  if (!ok) {
    Serial.print("configuring device failed");
    initialized = false;
    // Leave LED on for failed init.
    digitalWrite(LED_PIN, HIGH);
  } 

}

// Time last message recieved.
unsigned long last_rx = 0;

// Time last hello sent.
unsigned long last_hello = 0;

// Time last ping sent.
unsigned long last_ping = 0;

// Target address for ping messages.
unsigned short ping_target = 0;


void loop() {
  RYLR_LoRaAT_Message *message;

  if (!initialized) {
    return;
  }

  unsigned long now = millis();
  if (now - last_rx > 1000) {
    // Turn off LED 1 second after RX message.
    digitalWrite(LED_PIN, LOW);
  }

  // Broadcast hello 2 per min
  if (now - last_hello > 30000) {
    Serial.println("Send HELLO");
    last_hello = now;
    rylr.startTxMessage();
    rylr.addTxData("HELLO");
    rylr.sendTxMessage(0);
  }

  // Send a ping 1 per 30 sec
  if (now - last_ping > 30000 && ping_target != 0) {
    Serial.print("Send PING to: ");
    Serial.println(ping_target);

    last_ping = now;
    rylr.startTxMessage();
    rylr.addTxData("PING");
    rylr.sendTxMessage(ping_target);
  }

  // Handle any received messages
  if ((message = rylr.checkMessage())) {
    // Turn on LED on message RX
    last_rx = millis();
    digitalWrite(LED_PIN, HIGH);

    Serial.print("RX message from: ");
    Serial.print(message->from_address);
    Serial.print(", rssi: ");
    Serial.print(message->rssi);
    Serial.print(", snr: ");
    Serial.print(message->snr);
    Serial.print(", data: ");
    Serial.println(message->data);

    // Handle HELLO message
    if (strcmp(message->data, "HELLO") == 0) {
      // Ping target is sender of last hello message.
      ping_target = message->from_address;
    }

    // Handle PING message
    if (strcmp(message->data, "PING") == 0) {
      Serial.print("RX PING - send PONG to : ");
      Serial.println(message->from_address);
      rylr.startTxMessage();
      rylr.addTxData("PONG");
      rylr.sendTxMessage(message->from_address);
    }
  }
}

