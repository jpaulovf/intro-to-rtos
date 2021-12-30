/* Intro to RTOS
 *  "Task" demo
 *  
 *  Printing text to serial port
 *  
 *  Based on the following lesson:
 *  https://tinyurl.com/ytt6zj9t
 *  
 */

// Config ESP32 to use one core
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// String to print
const char msg[] = "Jill Sandwich";

// Task handles
static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_2 = NULL;

/* ======================== tasks =========================== */

// Task1: print to serial - char by char. Priority = low
// We use char by char just to see the task being interrupted
//  by a high-priority task
void startTask1(void *parameter) {

  // Number of chars in string
  int msg_len = strlen(msg);

  // Print string to serial port
  while(1) {
    Serial.println();
    for (int i=0; i<msg_len; i++) {
      Serial.print(msg[i]);
    }
    Serial.println();
    vTaskDelay(1000/portTICK_PERIOD_MS); // 1-sec Blocked state
  }
}

// Task2: print '*' to serial. Every 100ms. Priority = high
void startTask2(void *parameter) {

  while(1) {
    Serial.print("*");
    vTaskDelay(100/portTICK_PERIOD_MS);
  }
   
}

/* ============================================================== */

// setup() and loop() running as task with priority=1 @ core 1 

void setup() {
  
  // Config Serial port @ low baud rate
  Serial.begin(300);

  // Wait 1 sec for Seria lto start
  // We don't want to miss any Serial output!
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("--- FreeRTOS task demo ---");

  // Printing self core and priority
  Serial.print("setup() and loop() running on core ");
  Serial.print(xPortGetCoreID());
  Serial.print(" with priority ");
  Serial.println(uxTaskPriorityGet(NULL));

  // Starting other tasks

  // Lower priority
  xTaskCreatePinnedToCore(
    startTask1,     // Function to be caleld
    "Task 1",       // Task name
    1024,           // Stack size
    NULL,           // Parameters  
    1,              // Priority
    &task_1,        // Handle
    app_cpu         // Core             
  );

  // Higher priority
    xTaskCreatePinnedToCore(
    startTask2,     // Function to be caleld
    "Task 2",       // Task name
    1024,           // Stack size
    NULL,           // Parameters  
    2,              // Priority
    &task_2,        // Handle
    app_cpu         // Core             
  );

}

void loop() {
  
  // Suspends the higher priority task by 2 secs then resumes
  // Do this 3 times!
  for (int i=0; i<3; i++) {
    vTaskSuspend(task_2);
    vTaskDelay(2000/portTICK_PERIOD_MS);
    vTaskResume(task_2);
    vTaskDelay(2000/portTICK_PERIOD_MS);
  }

  // Deleting lower priority task
  // Check if the handler is NULL beforehand
  // Deleting a NULL task may be disastrous
  if (task_1 != NULL) {
    vTaskDelete(task_1);
    task_1 = NULL;
  }


}
