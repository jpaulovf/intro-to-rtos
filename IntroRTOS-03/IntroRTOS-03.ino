/* Intro to RTOS
 *  "Stack overflow" demo
 *  
 *  Stack overflow tests when creating tasks
 *  
 *  Based on the following lesson
 *  https://tinyurl.com/5a6djh8k
 */
 
// Config ESP32 to use one core 
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif 

/* If the heap size is small enough, an overflow will
 *  occur and the ESP32 will reboot as a safe measurement
 */
//#define TASK_HEAP_SIZE  1024  /* Using 1 KB, the overflow is observed */
#define TASK_HEAP_SIZE  1500  /* The minimum size required is ~ 1200. Using a little more */

// Test task: do something with an array
void testTask(void *parameter) {
  while (1) {

    int a = 1;  
    int b[100]; // sizeof(int) * 100 = 400 bytes

    // When creating a task, we have approx. 768 bytes of overhead
    // We then have 256 bytes remaining for the stack

    // Here, we need to do something with the array. Otherwise, the
    //  compiler will optimize it out
    for (int i=0; i<100; i++) {
      b[i] = a + i;
    }
    Serial.println(b[0]);

    // Printing the remaining bytes on stack memory in words
    // Multiply it by 4 to get the number of bytes
    Serial.print("High water mark (words): ");
    Serial.println(uxTaskGetStackHighWaterMark(NULL));

    // Printing the number of free heap memory (bytes)
    Serial.print("Free heap before malloc (bytes): ");
    Serial.println(xPortGetFreeHeapSize());

    // Dynamically allocate some memory
    int *ptr = (int*) pvPortMalloc(1024*sizeof(int));

    // ptr is null when pvPortMalloc cannot allocate
    if (ptr == NULL) {
      Serial.println("Not enough memory!");
    }
    else {
      // Doing something with the memory
      for (int i=0; i<1024; i++) {
        ptr[i] = 3;
      }
    }

    // Printing free heap after malloc
    Serial.print("Heap after malloc (bytes): ");
    Serial.println(xPortGetFreeHeapSize());

    // Freeing memory after use
    vPortFree(ptr);

    vTaskDelay(100/portTICK_PERIOD_MS);
    
  }
}

void setup() {
  
  Serial.begin(115200);

  vTaskDelay(1000/portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Memory Demo---");

  // Start task
  xTaskCreatePinnedToCore(testTask,
                          "Test Task",
                           TASK_HEAP_SIZE,  // Only 1 KB for this task
                           NULL,
                           1,
                           NULL,
                           app_cpu);

   // Deleting 'setup' and 'loop' tasks. only 1 task running! 
   vTaskDelete(NULL);

}

void loop() {
  // put your main code here, to run repeatedly:

}
