// If your target is limited in memory remove this macro to save 10K RAM
#define EIDSP_QUANTIZE_FILTERBANK   0

/**
   Define the number of slices per model window. E.g. a model window of 1000 ms
   with slices per model window set to 4. Results in a slice size of 250 ms.
   For more info: https://docs.edgeimpulse.com/docs/continuous-audio-sampling
*/

/* Includes ---------------------------------------------------------------- */
//#include <speech-recognition_inferencing.h>
//#include <speech_recognition_v2_inferencing.h>
#include <enzus-project-1_inferencing.h>

/** Audio buffers, pointers and selectors */
typedef struct {
  signed short *buffers[2];
  unsigned char buf_select;
  unsigned char buf_ready;
  unsigned int buf_count;
  unsigned int n_samples;
} inference_t;

static inference_t inference;
static bool record_ready = false;
static signed short *sampleBuffer;
static bool debug_nn = false; // Set this to true to see e.g. features generated from the raw signal
static int print_results = -(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW);


/**
   @brief      Printf function uses vsnprintf and output using Arduino Serial

   @param[in]  format     Variable argument list
*/
void ei_printf(const char *format, ...) {
  static char print_buf[1024] = { 0 };

  va_list args;
  va_start(args, format);
  int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
  va_end(args);

  if (r > 0) {
    Serial.write(print_buf);
  }
}

/**
   @brief      PDM buffer full callback
               Get data and call audio thread callback
*/
uint32_t lastFetchtimer = 0;
static void pdm_data_ready_inference_callback(size_t bytesRead)
{
      for (int i = 0; i<bytesRead; i++) {
        inference.buffers[inference.buf_select][inference.buf_count++] = (int16_t)(dataBuffer[i]/65536);

        if (inference.buf_count >= inference.n_samples) {
//          Serial.print("Duration between complete samples: ");Serial.println(millis()-lastFetchtimer);
//          Serial.println(inference.buf_count);
          lastFetchtimer = millis();
          inference.buf_select ^= 1;
          inference.buf_count = 0;
          inference.buf_ready = 1;
          //Serial.println("BUFF READY...");
        }
      }
}

/**
   @brief      Init inferencing struct and setup/start PDM

   @param[in]  n_samples  The n samples

   @return     { description_of_the_return_value }
*/
static bool microphone_inference_start(uint32_t n_samples)
{
  inference.buffers[0] = (signed short *)malloc(n_samples * sizeof(signed short));

  if (inference.buffers[0] == NULL) {
    return false;
  }

  inference.buffers[1] = (signed short *)malloc(n_samples * sizeof(signed short));

  if (inference.buffers[1] == NULL) {
    free(inference.buffers[0]);
    return false;
  }

  sampleBuffer = (signed short *)malloc((n_samples >> 1) * sizeof(signed short));

  if (sampleBuffer == NULL) {
    free(inference.buffers[0]);
    free(inference.buffers[1]);
    return false;
  }

  inference.buf_select = 0;
  inference.buf_count = 0;
  inference.n_samples = n_samples;
  inference.buf_ready = 0;

  record_ready = true;

  return true;
}

/**
   @brief      Wait on new data

   @return     True when finished
*/
static bool microphone_inference_record(void)
{
  bool ret = true;

  if (inference.buf_ready == 1) {
    ei_printf(
      "Error sample buffer overrun. Decrease the number of slices per model window "
      "(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW)\n");
    ret = false;
  }

  while (inference.buf_ready == 0) {
    delay(1);
  }

  inference.buf_ready = 0;

  return ret;
}

/**
   Get raw audio signal data
*/
static int microphone_audio_signal_get_data(size_t offset, size_t length, float *out_ptr)
{
  numpy::int16_to_float(&inference.buffers[inference.buf_select ^ 1][offset], out_ptr, length);
  return 0;
}

/**
   @brief      Stop PDM and release buffers
*/
static void microphone_inference_end(void)
{
  //PDM.end();
  free(inference.buffers[0]);
  free(inference.buffers[1]);
  free(sampleBuffer);
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_MICROPHONE
#error "Invalid model for current sensor."
#endif
