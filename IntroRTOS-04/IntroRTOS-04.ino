/*  Intro to RTOS
 *   
 *  Queue example 
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

// Time constants to tinker with
static const int wait_1000ms = 1000/portTICK_PERIOD_MS;
static const int wait_500ms = 500/portTICK_PERIOD_MS;

// Number of items the queue can hold
static const uint8_t msg_queue_len = 5;

// Queue: global, so both tasks can access
static QueueHandle_t msg_queue;

/* ---- Task 1: wait for queue items and print them --- */
void taskPrintMessages(void *parameters) {

  int item;

  while (1) {

    // Check for queued values (handler, element addr, timeout - no. of ticks)
    if (xQueueReceive(msg_queue, (void *) &item, 0) == pdTRUE) {
//      Serial.println(item);
    }
    Serial.println(item);

    vTaskDelay(wait_500ms);
    
  }
  
}

void setup() {

  Serial.begin(115200);

  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Queue Demo---");

  // Create queue (for integers) -- (no. of items, size of each item)
  msg_queue = xQueueCreate(msg_queue_len, sizeof(int));

  // Start task, priority = 1
  xTaskCreatePinnedToCore(taskPrintMessages, "Print Messages", 1024, NULL, 1, NULL, app_cpu);
  
}

void loop() {

  static int num = 0;
  
  // Adding element to queue, if possible
  if (xQueueSend(msg_queue, (void *) &num, 10) != pdTRUE) {
    // Just ignores the operation if queue is full
    Serial.println("Queue full!");
  }

  num++;

  vTaskDelay(wait_1000ms);
}
