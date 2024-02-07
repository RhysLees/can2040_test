#include <iostream>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "modules/can/CAN.hpp"

#define BUTTONONE 17
#define BUTTONTWO 16
#define rxLed 18
#define txLed 19
#define idReceived 20

#define RX_PIN 4
#define TX_PIN 5

#define RX_ID 0x200
#define TXONE_ID 0x201
#define TXTWO_ID 0x202

// #define RX_ID 0x201
// // #define RX_ID 0x202
// #define TXONE_ID 0x200
// #define TXTWO_ID 0x200

volatile bool BUTTONONE_pressed = false;
volatile bool BUTTONTWO_pressed = false;

volatile bool BUTTONONE_state = false;
volatile bool BUTTONTWO_state = false;

static void BUTTON_isr(uint gpio, uint32_t events)
{
    if (gpio == BUTTONONE)
    {
        BUTTONONE_pressed = true;
    }

    if (gpio == BUTTONTWO)
    {
        BUTTONTWO_pressed = true;
    }
}

void debounce()
{
    sleep_ms(20); // Adjust the delay as needed for debouncing
}

int main()
{
    stdio_init_all();

    gpio_init(BUTTONONE);
    gpio_set_dir(BUTTONONE, GPIO_IN);
    gpio_pull_down(BUTTONONE);

    gpio_init(BUTTONTWO);
    gpio_set_dir(BUTTONTWO, GPIO_IN);
    gpio_pull_down(BUTTONONE);

    gpio_init(rxLed);
    gpio_set_dir(rxLed, GPIO_OUT);

    gpio_init(txLed);
    gpio_set_dir(txLed, GPIO_OUT);

    gpio_init(idReceived);
    gpio_set_dir(idReceived, GPIO_OUT);

    // CAN
    printf("Starting Initialization of CAN \n");
    CAN can(RX_PIN, TX_PIN, RX_ID);
    can.setup();
    printf("Initialized CAN \n");

    // Set up interrupt for button pressess
    gpio_set_irq_enabled_with_callback(BUTTONONE, GPIO_IRQ_EDGE_RISE, true, &BUTTON_isr);
    gpio_set_irq_enabled(BUTTONTWO, GPIO_IRQ_EDGE_RISE, true);

    while (true)
    {
        // Check if the bUTTONONE was pressed
        if (BUTTONONE_pressed)
        {
            // Debouncing
            debounce();

            // Toggle the state
            BUTTONONE_state = !BUTTONONE_state;

            // Perform action based on the toggle state
            if (BUTTONONE_state)
            {
                can.transmit(TXONE_ID, 0x01);
            }
            else
            {
                can.transmit(TXONE_ID, 0x00);
            }

            // Reset the flag
            BUTTONONE_pressed = false;
        }

        // Check if the bUTTONONE was pressed
        if (BUTTONTWO_pressed)
        {
            // Debouncing
            debounce();

            // Toggle the state
            BUTTONTWO_state = !BUTTONTWO_state;

            // Perform action based on the toggle state
            if (BUTTONTWO_state)
            {
                can.transmit(TXTWO_ID, 0x01);
            }
            else
            {
                can.transmit(TXTWO_ID, 0x00);
            }

            // Reset the flag
            BUTTONTWO_pressed = false;
        }

        sleep_ms(100); // Adjust the delay as needed

        gpio_put(rxLed, 0);
        gpio_put(txLed, 0);
    }
}
