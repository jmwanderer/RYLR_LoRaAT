# Library for Reyax LoRa devices with a UART
An Arduino library for sending and receiving data messages
using Reyax UART based LoRa devices.

## Hardware
- RYLR998
- RYLR993

## Features

The RYLR998 and RYLR993 devices have a simple and covenient UART interface for configuration and
sending and recieving data.  For example:

```AT+SEND=2,5,HELLO\r\n```

Is received as:

```RCV=1,5,HELLO,-5,11\r\n```

This interface makes it easy to manually drive the device via an appropriate terminal
program and programmatically with simple code. However there are a few issues that make
programming a little tricky, and this library helps address these:

- Read and parse device command response codes
- Parse received messages
- Calculate length of messages to send
- Handle messages in the buffer received ahead of a command response
- Handle binary data in messages

## Installation

1. Download the library as a ZIP file or clone from GitHub.
2. In Arduino IDE: Sketch → Include Library → Add .ZIP Library, select the downloaded ZIP.
3. Or copy the RYLR_LoRaAT folder to your Arduino libraries directory.

## Quick Start Example
```
#include <Arduino.h>
#include "rylr_loraat.h"

RYLR_LoRaAT rylr;

void setup() {
  Serial.begin(115200);

  // Connect to LoRa device connected to Serial1
  Serial1.begin(115200);
  rylr.setSerial(&Serial1);

  // Set parameters of LoRa device.
  rylr.setAddress(1);

  // Broadcast a HELLO message
    rylr.startTxMessage();
    rylr.addTxData("HELLO");
    rylr.sendTxMessage(0);
}


void loop() {
  RYLR_LoRaAT_Message *message;

  // Handle received messages
  if ((message = rylr.checkMessage())) {
    Serial.print("RX message from: ");
    Serial.print(message->from_address);
    Serial.print(", data: ");
    Serial.print(message->data);
    Serial.println();
 }
}

```

# Examples

The examples work best with two or more radios 

## Simple
Simple.ino broadcasts HELLO messages and listens for messages.

## HelloPing
HelloPing.ino:
- User configures local address via serial port
- Sends periodic HELLO messages
- Sends periodic PING to last HELLO sender
- Replies to PINGs with a PONG message
- Flashes LED on packet RX

## Ping
Ping.ino:
- User configures local address via serial port
- Broadcast PING on button push
- Replies to PINGs with a PONG message
- Flashes LED on packet RX

## API

### Start
```
RYLR_LoRaAT rylr;

// Assums LoRa device connected to Serial1 pins.
Serial1.begin(115200);
rylr.setSerial(&Serial1);
rylr.setAddress(local_address);
rylr.setRFPower(14);
```

### Send Data
```
rylr.startTxMessage();
rylr.addTxData("HELLO");
rylr.sendTxMessage(0);
```

### Receive Data
```
RYLR_LoRaAT_Message *message;
if ((message = rylr.checkMessage())) {
  Serial.print("Recevied message from ");
  Serial.print(message->from_address);
  Serial.print(" length ");
  Serial.print(message->data_len);
  Serial.print(" : ")
  Serial.println(message->data);
}
```
