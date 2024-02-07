#include "CAN.hpp"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

extern "C"
{
#include "can2040.h"
}

CAN::CAN(uint32_t sys_clock, uint32_t bitrate, uint8_t rxPin, uint8_t txPin, uint8_t txId)
    : sys_clock(sys_clock), bitrate(bitrate), rxPin(rxPin), txPin(txPin), txId(txId) {}

void CAN::setup()
{
    can2040_setup(&handle, 1); // Pass the address of the handle
    can2040_callback_config(&handle, &CAN::canCallback);

    // Enable irqs
    irq_set_exclusive_handler(PIO1_IRQ_0, &CAN::pioIrqHandler);
    irq_set_priority(PIO1_IRQ_0, 1);
    irq_set_enabled(PIO1_IRQ_0, true);

    can2040_start(&handle, sys_clock, bitrate, rxPin, txPin);
}

void CAN::transmit(uint8_t data)
{
    CANMsg msg = {}; // Initialize the struct with zeros
    msg.dlc = 1;
    msg.id = txId; // Use the dynamic transmit ID
    msg.data[0] = data;

    if (can2040_check_transmit(&handle))
    {
        int res = can2040_transmit(&handle, &msg);
        // Handle transmission result as needed
    }
}

static void pioIrqHandler(void)
{
    can2040_pio_irq_handler(&handle);
}

void CAN::canCallback(CANHandle *cd, uint32_t notify, CANMsg *msg)
{
    if (notify == CAN2040_NOTIFY_RX)
    {
        printf("xfguo: recv msg: (id: %0x, size: %0x, data: %0x, %0x)\n", msg->id & 0x7ff, msg->dlc, msg->data32[0], msg->data32[1]);

        if (msg->data32[0] == 0x01)
        {
            gpio_put(20, 1);
        }
        else
        {
            gpio_put(20, 0);
        }
    }
    else if (notify == CAN2040_NOTIFY_TX)
    {
        printf("xfguo: confirmed tx msg: (id: %0x, size: %0x, data: %0x, %0x)\n", msg->id & 0x7ff, msg->dlc, msg->data32[0], msg->data32[1]);
    }
    else if (notify & CAN2040_NOTIFY_ERROR)
    {
        printf("xfguo: error(%d) on msg: (id: %0x, size: %0x, data: %0x, %0x)\n", notify & 0xff, msg->id & 0x7ff, msg->dlc, msg->data32[0], msg->data32[1]);
    }
    gpio_put(18, 1);
}