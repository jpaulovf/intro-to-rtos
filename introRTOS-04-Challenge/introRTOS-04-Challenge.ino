/* Intro to RTOS
 *  
 * Challenge 04 - "Queue example" 
 * 
 * Create 2 tasks and 2 queues
 * 
 * TASK A: Terminal
 * - Prints items from queue 2
 * - Reads inputs from serial
 * - Echoes the characters from input
 * - If input is "delay nnn", sends the number nnn to queue 1
 * 
 * TASK B: LEDControl
 *  - Stores the item from queue 1 in the variable t
 *  - Blinks an LED with period = t
 *  - When the LED blinks 100 times, sends the string "Blinked" to queue 2
 *  - Optional: also prints the number of times the LED blinked
 * 
 *  Based on this lesson
 *  https://tinyurl.com/35psthjz
 */

// Configure ESP32 to use only 1 core
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Input string buffer max size
static const int string_max_size = 255;

// Number of items in queues
static const uint8_t queue_len = 10;

// Queue 1: blinking period, ms
static QueueHandle_t period_queue;

// Queue 2: message queue
static QueueHandle_t msg_queue;

// Message: struct with a string and a number
typedef struct {
  String message;
  int blink_count;
} led_message_t;

// LED pin
static const int led_pin = 2;

/* --- Task A: Terminal --- */
void taskTerminal(void *parameters) {

  char received_char;
  int period_ms;
  char input_buffer[string_max_size];
  memset(input_buffer, 0, string_max_size);
  int count = 0;

  char *cptr;

  led_message_t item;
  
  while (1) {

    // Check for received messages from queue 2
    if (xQueueReceive(msg_queue, (void *) &item, 0) == pdTRUE) {
      Serial.println();
      Serial.print(item.message);
      Serial.print(" ");
      Serial.println(item.blink_count);
    }

    // Receiving characters from serial and echoing
    if (Serial.available() > 0) {
      received_char = Serial.read();
      // Echoing
      Serial.print(received_char);
      
      if (count < string_max_size - 1) {
        input_buffer[count] = received_char;
        count++;
      }

      if (received_char == '\n') {

        input_buffer[count] = '\0';

        // Parsing message
        if (strncmp(input_buffer, "delay ", 6) == 0) {
          cptr = strtok(input_buffer, " ");
          cptr = strtok(NULL, " ");
          period_ms = (int) strtol(cptr, NULL, 10);
        }

        if (period_ms > 0) {
          // Queueing blink period
          if (xQueueSend(period_queue, (void *) &period_ms, 10) != pdTRUE) {
            Serial.println("Queue 1 full!");
          }
        }

        memset(input_buffer, 0, string_max_size);
        count = 0;
      }    
    } 
  }
}

/* --- Task B: LEDControl --- */
void taskLEDControl(void *parameters) {

  // LED blink rate (initial = 100 ms)
  int t = 100;
  int count = 0;
  led_message_t item;

  item.message = "Blinked";
  
  while (1) {

    // Reading period from queue 1
    if (xQueueReceive(period_queue, (void *) &t, 0) == pdTRUE) {
      
    }

    digitalWrite(led_pin, !digitalRead(led_pin));
    vTaskDelay(t / portTICK_PERIOD_MS);
    count++;

    // Checking for multiples of 100
    if (count % 100 == 0) {
      item.blink_count = count;
      if (xQueueSend(msg_queue, (void *) &item, 10) != pdTRUE) {
        Serial.println("Queue 2 full!");
      }
    }
    
  }
  
}


void setup() {

  pinMode(led_pin, OUTPUT);
  
  Serial.begin(115200);

  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("--- FreeRTOS Queue Challenge ---");

  // Creating queues
  period_queue = xQueueCreate(queue_len, sizeof(int));
  msg_queue = xQueueCreate(queue_len, sizeof(led_message_t));

  // Start Tasks
  xTaskCreatePinnedToCore(taskTerminal, "Terminal", 1024, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore(taskLEDControl, "LED Control", 1024, NULL, 1, NULL, app_cpu);

  // Deleting setup() and loop() tasks
  vTaskDelete(NULL);
}

void loop() {
  // put your main code here, to run repeatedly:

}
