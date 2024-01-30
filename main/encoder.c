#include "encoder.h"

// Interrupt service routine for encoder channel A
void IRAM_ATTR encoderChannelAISR(void *arg)
{
    flagChannelA = true;
}

void update_ChannelA()
{
    bool channelAState = gpio_get_level(ENCODER_CHANNEL_A_PIN);

    if ((lastChannelAState == 0) && (channelAState == 1))
    {
        if (lastChannelBState == 0)
        {
            encoderCount++;
        }
        else
        {
            encoderCount--;
        }
    }

    lastChannelAState = channelAState;
}

// Interrupt service routine for encoder channel B
void IRAM_ATTR encoderChannelBISR(void *arg)
{
    flagChannelB = true;
}

void update_ChannelB()
{
    bool channelBState = gpio_get_level(ENCODER_CHANNEL_B_PIN);

    if ((lastChannelBState == 0) && (channelBState == 1))
    {
        if (lastChannelAState == 1)
        {
            encoderCount++;
        }
        else
        {
            encoderCount--;
        }
    }

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
}