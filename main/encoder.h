#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Define GPIO pins for encoder channels
#define ENCODER_CHANNEL_A_PIN GPIO_NUM_4
#define ENCODER_CHANNEL_B_PIN GPIO_NUM_5

// Encoder info
#define counts_per_rev 12
#define distance_per_rev 32

// Variables to store encoder state
volatile int32_t encoderCount = 0;
volatile bool lastChannelAState = 0;
volatile bool lastChannelBState = 0;

// Flags
volatile bool flagChannelA = false;
volatile bool flagChannelB = false;

void IRAM_ATTR encoderChannelAISR(void *arg);
void update_ChannelA();
void IRAM_ATTR encoderChannelBISR(void *arg);
void update_ChannelB();

void initializeEncoder();