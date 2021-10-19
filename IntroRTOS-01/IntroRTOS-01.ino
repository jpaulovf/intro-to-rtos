// Using only one core
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// LED Pin
static const int led_pin = 2;

// Task: Blink an LED -- 100 ms
void toggleLED(void * parameter) {
  while(1) {
    digitalWrite(led_pin, !digitalRead(led_pin));
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// Another Task: Blink an LED -- 110 ms
void anotherToggleLED(void *parameter) {
  while(1) {
    digitalWrite(led_pin, !digitalRead(led_pin));
    vTaskDelay(110 / portTICK_PERIOD_MS);
  }
}

void setup() {
  
  pinMode(led_pin, OUTPUT);

  // Task to run forever
  xTaskCreatePinnedToCore ( // Vanilla FreeRTOS: xTaskCreate()
    toggleLED,              // Function to be called
    "Toggle LED",           // Task name
    1024,                   // Stack size (ESP32:bytes, FreeRTOS: words)
    NULL,                   // Function parameter
    1,                      // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,                   // Task handle
    app_cpu);               // ESP32 only: Core to run

  // If using FreeRTOS, call vTaskStartScheduler() in main

  // Another task
  xTaskCreatePinnedToCore ( // Vanilla FreeRTOS: xTaskCreate()
    anotherToggleLED,       // Function to be called
    "Another Toggle LED",   // Task name
    1024,                   // Stack size (ESP32:bytes, FreeRTOS: words)
    NULL,                   // Function parameter
    1,                      // Task priority (0 to configMAX_PRIORITIES - 1)
    NULL,                   // Task handle
    app_cpu);               // ESP32 only: Core to run

}

void loop() {
  // put your main code here, to run repeatedly:

}
