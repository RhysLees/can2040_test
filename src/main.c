#include <stdio.h>
#include "pico/stdlib.h"
#include "can2040.h"
#include "hardware/gpio.h"

#define BUTTON 17
#define rxLed 18
#define txLed 19
#define idReceived 20

#define RX_PIN 4
#define TX_PIN 5

#define RX_ID 0x201
#define TX_ID 0x200

typedef struct can2040 CANHandle;
typedef struct can2040_msg CANMsg;

static CANHandle can1;

static void can2040_cb1(CANHandle *cd, uint32_t notify, CANMsg *msg)
{
    if (notify == CAN2040_NOTIFY_RX)
    {
        printf("recv msg: (id: %0x, size: %0x, data: %0x, %0x)\n", msg->id & 0x7ff, msg->dlc, msg->data32[0], msg->data32[1]);

        if (msg->data32[0] == 0x01)
        {
            gpio_put(idReceived, 1);
        }
        else
        {
            gpio_put(idReceived, 0);
        }
    }
    else if (notify == CAN2040_NOTIFY_TX)
    {
        printf("confirmed tx msg: (id: %0x, size: %0x, data: %0x, %0x)\n", msg->id & 0x7ff, msg->dlc, msg->data32[0], msg->data32[1]);
    }
    else if (notify & CAN2040_NOTIFY_ERROR)
    {
        printf("error(%d) on msg: (id: %0x, size: %0x, data: %0x, %0x)\n", notify & 0xff, msg->id & 0x7ff, msg->dlc, msg->data32[0], msg->data32[1]);
    }
    gpio_put(rxLed, 1);
}

static void PIO1_IRQHandler(void)
{
    can2040_pio_irq_handler(&can1);
}

void canbus_setup()
{
    uint32_t sys_clock = 125000000, bitrate = 125000; // 250000;

    // Setup canbus
    can2040_setup(&can1, 1);
    can2040_callback_config(&can1, can2040_cb1);

    // Enable irqs
    irq_set_exclusive_handler(PIO1_IRQ_0, PIO1_IRQHandler);
    irq_set_priority(PIO1_IRQ_0, 1);
    irq_set_enabled(PIO1_IRQ_0, true);

    // Start canbus
    can2040_start(&can1, sys_clock, bitrate, RX_PIN, TX_PIN);
}

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
    canbus_setup();
    printf("Initialized CAN1\n");

    sleep_ms(1000);
    while (true)
    {
        CANMsg msg = {0};
        msg.dlc = 1;
        msg.id = TX_ID;

        if (gpio_get(BUTTON))
        {
            msg.data[0] = 0x01;
        }
        else
        {
            msg.data[0] = 0x00;
        }

        if (can2040_check_transmit(&can1))
        {
            int res = can2040_transmit(&can1, &msg);
            gpio_put(rxLed, 0);
            gpio_put(txLed, 1);
            sleep_ms(100);
            gpio_put(rxLed, 0);
            gpio_put(txLed, 0);
            sleep_ms(100);
            printf("Transmit result: %d\n", res);
        }
        sleep_ms(100);
    }
}