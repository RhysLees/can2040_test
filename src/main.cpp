#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "modules/can/CAN.hpp"

#define BUTTON 17
#define rxLed 18
#define txLed 19
#define idReceived 20

#define RX_PIN 4
#define TX_PIN 5

#define RX_ID 0x200
#define TX_ID 0x201

int main()
{
    stdio_init_all();

    gpio_init(BUTTON);
    gpio_set_dir(BUTTON, GPIO_IN);
    gpio_pull_down(BUTTON);

    gpio_init(rxLed);
    gpio_set_dir(rxLed, GPIO_OUT);

    gpio_init(txLed);
    gpio_set_dir(txLed, GPIO_OUT);

    gpio_init(idReceived);
    gpio_set_dir(idReceived, GPIO_OUT);

    sleep_ms(100);
    printf("Startup delay over\n");

    printf("Starting Initialization of CAN\n");
    CAN can(125000000, 125000, RX_PIN, TX_PIN, (uint8_t)TX_ID);
    can.setup();
    printf("Initialized CAN1\n");

    sleep_ms(1000);
    while (true)
    {
        if (gpio_get(BUTTON))
        {
            can.transmit(0x01);
        }
        else
        {
            can.transmit(0x00);
        }

        sleep_ms(100);
    }
}
