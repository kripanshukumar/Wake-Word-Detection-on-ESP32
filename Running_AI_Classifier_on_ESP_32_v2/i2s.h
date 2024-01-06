//=====================================================================================================================================

void i2s_install() {
//  const i2s_config_t i2s_config = {
//    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
//    .sample_rate = SAMPLE_RATE,
//    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
//    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
//    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
//    .intr_alloc_flags = 0, // default interrupt priority
//    .dma_buf_count = 8,
//    .dma_buf_len = 1000,
//    .use_apll = false
//  };
//
//  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

  i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
        .channel_format = I2S_CHANNEL_FMT_ALL_RIGHT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 1000,
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    
}

//=====================================================================================================================================

void i2s_setpin() {
//  const i2s_pin_config_t pin_config = {
//    .bck_io_num = I2S_SCK,
//    .ws_io_num = I2S_WS,
//    .data_out_num = -1,
//    .data_in_num = I2S_SD
//  };
//
//  i2s_set_pin(I2S_PORT, &pin_config);

  i2s_pin_config_t pin_config;
    pin_config.bck_io_num = I2S_PIN_NO_CHANGE;
    pin_config.ws_io_num = PIN_CLK;
    pin_config.data_out_num = I2S_PIN_NO_CHANGE;
    pin_config.data_in_num = PIN_DATA;

    i2s_set_pin(I2S_NUM_0, &pin_config);
}

//=====================================================================================================================================

void reader(void *pvParameters) {

  size_t bytes_read;
  //uint32_t t = millis();
  //int i = 0;
  while (1) {
    i2s_read(I2S_NUM_0, dataBuffer, sizeof(int32_t) * 1000, &bytes_read, portMAX_DELAY);
    //Serial.print("Real Bytes Read: ");Serial.println(bytes_read);
    if (bytes_read > 0) {
      pdm_data_ready_inference_callback(bytes_read/4);
      int16_t temp = 0;
      for (int i = 0; i < 1000; i++) {
        temp = dataBuffer[i]/65536;
        Serial.println(temp);
      }
      
    }
  }
}

//=====================================================================================================================================

void I2S_INIT() {

  delay(500);
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);
  delay(500);

  // Create a task that will read the data
  xTaskCreatePinnedToCore (
    reader, /* Task function. */
    "ADC_reader", /* String with name of task. */
    20480, /* Stack size in bytes. */
    NULL, /* Parameter passed as input of the task */
    configMAX_PRIORITIES -2, /* Priority of the task. */
    NULL, /* Task handle. */
    1);
}

//
