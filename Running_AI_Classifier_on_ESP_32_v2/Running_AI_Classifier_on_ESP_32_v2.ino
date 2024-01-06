e #include "Variables.h"
#include "AI_MODEL.h"
#include "i2s.h"
#include "Algorithms.h"


/**
   @brief      Arduino setup function
*/
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("Edge Impulse Inferencing Demo");

  // summary of inferencing settings (from model_metadata.h)
  ei_printf("Inferencing settings:\n");
  ei_printf("\tInterval: %.2f ms.\n", (float)EI_CLASSIFIER_INTERVAL_MS);
  ei_printf("\tFrame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
  ei_printf("\tSample length: %d ms.\n", EI_CLASSIFIER_RAW_SAMPLE_COUNT / 16);
  ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) /
            sizeof(ei_classifier_inferencing_categories[0]));

  run_classifier_init();
  if (microphone_inference_start(EI_CLASSIFIER_SLICE_SIZE) == false) {
    ei_printf("ERR: Failed to setup audio sampling\r\n");
    return;
  }
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);
  I2S_INIT();
}

/**
   @brief      Arduino main function. Runs the inferencing loop.
*/

uint32_t timer = 0;
uint32_t t = 0;

void loop()
{
  bool m = microphone_inference_record();
  if (!m) {
    ei_printf("ERR: Failed to record audio...\n");
    return;
  }

  signal_t signal;
  signal.total_length = EI_CLASSIFIER_SLICE_SIZE;
  signal.get_data = &microphone_audio_signal_get_data;
  ei_impulse_result_t result = {0};

  EI_IMPULSE_ERROR r = run_classifier_continuous(&signal, &result, debug_nn);
  if (r != EI_IMPULSE_OK) {
    ei_printf("ERR: Failed to run classifier (%d)\n", r);
    return;
  }

  if (++print_results >= (EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW)) {
    // print the predictions
    ei_printf("Predictions ");
    ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
              result.timing.dsp, result.timing.classification, result.timing.anomaly);
    ei_printf(": \n");
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
      ei_printf("    %s: %.5f\n", result.classification[ix].label,
                result.classification[ix].value);
    }
    if (result.classification[1].value > 0.80 | averageProb(result.classification[1].value) > 0.45 ) {
      Serial.println("High....................................");
      digitalWrite(27, HIGH);
      if (millis() - b_timer > BUZZER_TIMER) {
        digitalWrite(26, HIGH);
      }
      t = millis();
    }
    else {
      if (millis() - t > 3000) {
        b_timer = millis();
        digitalWrite(27, LOW);
        digitalWrite(26, LOW);
      }
    }

#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif

    print_results = 0;
  }
}
