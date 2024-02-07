// CAN.h
#pragma once

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

extern "C"
{
#include "can2040.h"
}

typedef struct can2040 CANHandle;
typedef struct can2040_msg CANMsg;

class CAN
{
public:
    CAN(uint32_t sys_clock, uint32_t bitrate, uint8_t rxPin, uint8_t txPin, uint8_t txId);

    void setup();
    void transmit(uint8_t data);

private:
    static CANHandle handle;
    uint32_t sys_clock;
    uint32_t bitrate;
    uint8_t rxPin;
    uint8_t txPin;
    uint8_t txId;

    static void canCallback(CANHandle *cd, uint32_t notify, CANMsg *msg);
    static void pioIrqHandler(void);
};
