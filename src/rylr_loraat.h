// Copyright (c) James Wanderer. All rights reserved
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// Simple driver for Reyax LoRA devices with a UART interface
//

#ifndef __RYLR_LORAAT_H__
#define __RYLR_LORAAT_H__

#include <Arduino.h>

#define READ_TIMEOUT_MS 100                 // 100 ms
#define SEND_TIMEOUT_MS 1000                // 1 second
#define MAX_DATA_LEN 100                    // 100 bytes of data
#define MAX_TX_MSG MAX_DATA_LEN + 20
#define MAX_RX_MSG MAX_DATA_LEN + 28
#define RX_BUFFER_LEN (MAX_RX_MSG * 3)

// Utility function to prompt for a device address over
// the serial port;
int promptUserForAddress();

// Received message
// Points to data in the RX Buffer
// Valid until next operation.
struct RYLR_LoRaAT_Message
{
    uint8_t from_address;
    uint16_t data_len;
    char *data;
    int rssi;
    int snr;
};

class RYLR_LoRaAT 
{
    public:
        RYLR_LoRaAT();

        // Connect to a hardware serial device.
        void setSerial(HardwareSerial* serial) { this->serial = serial; }

        // Communicate with device. A return value of 0 means OK
        int checkStatus();

        // Device address: 0 to 65535
        int setAddress(uint16_t address);

        // RF Parameters
        int setRFParameters(uint8_t spread, uint8_t bandwidth, uint8_t coding_rate, uint8_t preamble);

        // RF power: 0 to 22 (14 to comply with CE cert)
        int setRFPower(uint8_t power);

        // Set password. Must be 00000001 - FFFFFFFF
        int setPassword(const char* password);

        // Recieve a message
        RYLR_LoRaAT_Message* checkMessage();

        // Send a message
        void startTxMessage();
        void addTxData(int len, const char* data);
        void addTxData(const char* data);
        void addTxData(int data);
        void addTxData(double data);
        int sendTxMessage(uint8_t to_address);

        // Debug functions
        void dumpMessage();
        void dumpStats();

    private:
        HardwareSerial* serial;

        // Read buffer and pointers
        char rx_buffer[RX_BUFFER_LEN];
        int rx_index;
        int read_index;

        // RX message descriptor
        RYLR_LoRaAT_Message rx_message;
        
        // TX Buffer and pointers
        char tx_buffer[MAX_TX_MSG];
        int tx_index;

        const char* RECV = "+RCV=";

        // Counters
        int overflow_count;
        int overwrite_count;
        int rx_error_count;
        int tx_message_count;
        int rx_message_count;

        const char* getResponse();
        int processInput();
        int resultValue(const char* valstr);
        int parseDataLength(int data_size);
        void resetRxBuffer();
};


#endif /* __RYLR_LORAAT_H__ */