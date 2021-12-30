/* Intro to RTOS
 * Challenge 03: "Passing a message"   
 * 
 * Creates 2 tasks:
 * TASK A
 * - Listens to the serial monitor
 * - If '\n' is received, store the chars on the heap
 * - Notifies task B
 * 
 * TASK B
 * - Waits for task A notification
 * - Prints stored message
 * - Frees the heap
 * 
 * Based on the following lesson
 *  https://tinyurl.com/5a6djh8k
 */

// Configuring ESP32 to use one core
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Maximum number of bytes to receive
#define STRING_BUFF_MAX 255

// Flag to signalize from task A to task B
static volatile uint8_t flag_tx_done = 0;

// Pointer to heap
static char *ptr = NULL;

/* TASK A: Receive from terminal */
void taskA(void *parameters) {

  // Number of characters received
  int count = 0;

  // Buffer
  char inputBuffer[STRING_BUFF_MAX];
  memset(inputBuffer, 0, STRING_BUFF_MAX);

  // Incoming character
  char c;
  
  while (1) {
    
    if (Serial.available() > 0) {
      // Read char from serial
      c = Serial.read();

      if (count < STRING_BUFF_MAX - 1) {
        inputBuffer[count] = c;
        count++;
      }

      if (c == '\n') {
        // Replace last character with \0
        inputBuffer[count-1] = '\0';

        if (flag_tx_done == 0) {
          // Allocate memory to copy the message
          ptr = (char *) pvPortMalloc(count * sizeof(char));
          configASSERT(ptr);
          // Copy inputBuffer to ptr
          memcpy(ptr, inputBuffer, count);
          // Notify TASK B that TX is done
          flag_tx_done = 1;
        }

        // Reset buffer
        memset(inputBuffer, 0, STRING_BUFF_MAX);
        count = 0;
      }
    }
  }
}

/* TASK B: Waits for notification / Prints */
void taskB (void *parameters) {
  while (1) {

    if (flag_tx_done == 1) {
      Serial.println(ptr);

      Serial.print("Free heap: ");
      Serial.print(xPortGetFreeHeapSize());
      Serial.println(" bytes");
      
      vPortFree(ptr);
      ptr = NULL;
      flag_tx_done = 0;
    }
  }
}

void setup() {

  Serial.begin(115200);

  vTaskDelay(1000 / portTICK_PERIOD_MS);

  Serial.println();
  Serial.println("---FreeRTOS Challenge 03---");

  // Starting tasks
  xTaskCreatePinnedToCore(taskA, "Task A", 1024, NULL, 1, NULL, app_cpu);
  xTaskCreatePinnedToCore(taskB, "Task B", 1024, NULL, 1, NULL, app_cpu);
  vTaskDelete(NULL);

}

void loop() {
  // put your main code here, to run repeatedly:
  
}
