#include <driver/i2s.h>

#define I2S_WS 15
#define I2S_SD 21
#define I2S_SCK 19

#define PIN_CLK 0
#define PIN_DATA 34

#define I2S_PORT I2S_NUM_0

#define LED_PIN 18
#define BUTTON 5

#define EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW 3
#define AVERAGING_SAMPLES 20
#define BUZZER_TIMER 30000                        // In Milliseconds

//================================================Variables.h========================================================
int SAMPLE_RATE = 16000;
int writeEnable = 0;
int valid[16] = {5,6,7,10,11,13,14,15,22,23,26,27,28,29,30,31};

//I2S Sampler
uint16_t buffer1[16000/EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW] = {0};
int32_t dataBuffer[16000/EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW] = {0};
uint8_t Sequence = 0;
uint8_t inputSeuence = 0;
float threshold = 0.80;

float circularBuffer[AVERAGING_SAMPLES] = {0};
int bufferPointer = 0;
float Accumulator = 0;
float average = 0;

uint32_t b_timer = 0;
