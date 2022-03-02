//Run a code on a desired core 

// RTOS Task 1
// Dual core programming
// Force a task to run on a particular core (0 or 1)

int c1 = 0;
void t1(void *par) {
  for(;;){
    Serial.print("Taskl running on core ");
    Serial.println(xPortGetCoreID());
    Serial.print ("Task 1 count:");
    Serial.print (cl++);
    Serial.printin();
    vTaskDelay (1000) ;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  xTaskCreatePinnedToCore (
    t1,       //Task function
    "Task 1", //Task Name
    1000,     //stack size
    NULL,     //parameter for the task
    1,        //Priority, gi
    NULL, //
    /f

}




Stack size

Parameter for the task
Priority, high for high no.
Task handle

Core
}
void loop() {
  // put your main code here, to run repeatedly:
}
