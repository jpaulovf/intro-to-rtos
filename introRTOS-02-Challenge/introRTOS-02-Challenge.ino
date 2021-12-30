/* Intro to RTOS Part3
 * Based on the following lesson:
 *  https://tinyurl.com/ytt6zj9t 
 *  
 *  
 *  Challenge
 *  Create 2 tasks
 *  One controls the blinking of an LED
 *  The other reads inputs from the user via Serial
 *   and controls the blinking rate of the LED
 */

// Config ESP32 to use one core
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

#define LED_BLINK_MAX_TIME_MS 5000

// LED pin
static const int led_pin = 2;

// LED blinking time, ms (default = 100)
int blink_rate_ms = 100;


/* ====================== Tasks =================== */

// Task 1: LED blinking
void taskLEDToggle(void *parameter) {

  while(1) {
    digitalWrite(led_pin, !digitalRead(led_pin));
    vTaskDelay(blink_rate_ms/portTICK_PERIOD_MS);
  }
  
}


// Task 2: Reading user's input
void taskReadSerial(void *parameter) {

  int input_number;

  while(1) {
    input_number = Serial.parseInt();
    if (input_number > 0 && input_number <= LED_BLINK_MAX_TIME_MS) {
      Serial.print("LED toggle period changed to ");
      Serial.print(input_number);
      Serial.println(" ms");
      blink_rate_ms = input_number;
    }
  }
  
}


/* ================================================ */

void setup() {

  pinMode(led_pin, OUTPUT);
  
  Serial.begin(115200);
  
  vTaskDelay(1000/portTICK_PERIOD_MS);
  Serial.setTimeout(200);
  Serial.println();
  Serial.println("--- FreeRTOS Challenge 02 ---");

  // Configuring tasks
  xTaskCreatePinnedToCore(
    taskLEDToggle,
    "LED task",
    1024,
    NULL,
    1,
    NULL,
    app_cpu  
  );

  xTaskCreatePinnedToCore(
    taskReadSerial,
    "Serial input task",
    1024,
    NULL,
    1,
    NULL,
    app_cpu  
  );
 
}

void loop() {
  
  // Nothing

}
