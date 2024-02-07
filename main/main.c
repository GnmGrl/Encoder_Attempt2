// #include "encoder.h"
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Define GPIO pins for encoder channels
#define ENCODER_CHANNEL_A_PIN GPIO_NUM_4
#define ENCODER_CHANNEL_B_PIN GPIO_NUM_5

// Variables to store encoder state
volatile int32_t encoderCount = 0;
volatile bool lastChannelAState = 0;
volatile bool lastChannelBState = 0;

// Flags
volatile bool flagChannelA = false;
volatile bool flagChannelB = false;

void IRAM_ATTR encoderChannelAISR(void *arg)
{
    flagChannelA = true;
}
void update_ChannelA()
{
    bool channelAState = !gpio_get_level(ENCODER_CHANNEL_A_PIN);
    int incr = 0;

    if ((lastChannelAState == false) && (channelAState == true))
    {
        incr = (lastChannelBState == false) ? 1 : -1; // if prev_BState == false, incr = 1, else incr = -1
    }
    else if ((lastChannelAState == true) && (channelAState == false))
    {
        incr = (lastChannelBState == true) ? 1 : -1;
    }
    encoderCount = encoderCount + incr;
    lastChannelAState = channelAState;
}

void IRAM_ATTR encoderChannelBISR(void *arg)
{
    flagChannelB = true;
}
void update_ChannelB()
{
    bool channelBState = !gpio_get_level(ENCODER_CHANNEL_B_PIN);
    int incr = 0;

    if ((lastChannelBState == false) && (channelBState == true))
    {
        incr = (lastChannelAState == true) ? 1 : -1;
    }
    else if ((lastChannelBState == true) && (channelBState == false))
    {
        incr = (lastChannelAState == false) ? 1 : -1;
    }
    encoderCount = encoderCount + incr;
    lastChannelBState = channelBState;
}

// Function to initialize GPIO and interrupts
void initializeEncoder()
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << ENCODER_CHANNEL_A_PIN) | (1ULL << ENCODER_CHANNEL_B_PIN),
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_INTR_ANYEDGE,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
    };
    gpio_config(&io_conf);

    // Install ISR service
    gpio_install_isr_service(0);

    // Hook ISR handler for both channels
    gpio_isr_handler_add(ENCODER_CHANNEL_A_PIN, encoderChannelAISR, (void *)ENCODER_CHANNEL_A_PIN);
    gpio_isr_handler_add(ENCODER_CHANNEL_B_PIN, encoderChannelBISR, (void *)ENCODER_CHANNEL_B_PIN);

    lastChannelAState = !gpio_get_level(ENCODER_CHANNEL_A_PIN); // logic on the pins start as one for some reason
    lastChannelBState = !gpio_get_level(ENCODER_CHANNEL_B_PIN);
}

void app_main(void)
{
    // Initialize
    initializeEncoder();

    // variables
    float distance = 0;
    uint32_t counts_per_rev = 12;
    uint32_t distance_per_rev = 32; // mm
    // float distance_per_rev = 1.25; //in

    while (1)
    {
        if (flagChannelA)
        {
            update_ChannelA();
            distance = ((float)encoderCount / (float)counts_per_rev) * (float)distance_per_rev; // convert from encoder counts to distance
            printf("Position: %ld   Distance: %f\n", encoderCount, distance);
            flagChannelA = false;
        }

        if (flagChannelB)
        {
            update_ChannelB();
            distance = ((float)encoderCount / (float)counts_per_rev) * (float)distance_per_rev; // convert from encoder counts to distance
            printf("Position: %ld   Distance: %f\n", encoderCount, distance);
            flagChannelB = false;
        }

        vTaskDelay(1);
        // vTaskDelay(pdMS_TO_TICKS(100));  //too much of a delay
    }
}