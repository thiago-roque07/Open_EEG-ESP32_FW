#include <Wire.h>
#include "Adafruit_ADS1015_mod.h"
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

Adafruit_ADS1115 ads;     // Use this for the 16-bit version 

const int alertPin = 5;
const int data_length = 256;

unsigned long count_new = 0;
unsigned long count_old = 0;
unsigned long time_diff;

volatile bool continuousConversionReady = false;
volatile bool buff_full = false;
bool onoff = LOW;

int value_array[data_length];
int result;
int array_index = 0;
int buff = 0;
int *ptr_data = value_array;
int channel = 0;
// LED pins
const int led1 = 2;
const int led2 = 4;

TaskHandle_t Task1;
TaskHandle_t Task2;

BluetoothSerial SerialBT;

void setup() {
  pinMode(alertPin,INPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  
  Serial.begin(115200);
  Serial.println("Hello!");
  
  SerialBT.begin("ESP32_BT"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    (void*)&result,        /* parameter of the task */
                    0,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 

  ads.setGain(GAIN_ONE);           // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  ads.begin();
  ads.setSPS(ADS1115_DR_860SPS);     
  ads.startContinuous_SingleEnded(1); 
  
  attachInterrupt(digitalPinToInterrupt(alertPin), continuousAlert, FALLING);
}

void continuousAlert() {
  continuousConversionReady = true;
  //Serial.println("oi");
  count_new = micros();
  time_diff = count_new-count_old;
  count_old = count_new;
}

//Task1code: blinks an LED every 1000 ms
void Task1code( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  Serial.println(data_length);

  for(;;){
    
//    for(int consume_index = 0; consume_index < data_length; consume_index++)
//    {
//      //Serial.println(*((int*)pvParameters)); 
//      //Serial.println ("1111111111111111111111111111111111111111111111111111111111111111111111111");
//      //pvParameters++;
//      //SerialBT.println (data_array[consume_index]);
//      SerialBT.println(*((int*)pvParameters)); 
//      //SerialBT.println ("1111111111111111111111111111111111111111111111111111111111111111111111111");
//    }
//    vTaskDelay(1070); //

      if (continuousConversionReady) {
        SerialBT.println(*((int*)pvParameters));
        //Serial.println(*((int*)pvParameters));
      }
  } 
  vTaskDelete( NULL );
}

void loop() {
  if (continuousConversionReady) {    
    
    value_array[array_index] = ((int) ads.getLastConversionResults());
    result = value_array[array_index];
    array_index = (array_index < data_length) ? array_index+1 : 0;
    
    continuousConversionReady = false;
 
    switch (channel) {
    case (0):
      channel = 1;
      break;
    case (1):
      channel = 2;
      break;
    case (2):
      channel = 0;
      break;
    }
    ads.changeChannelContinuous_SingleEnded(channel);
    
    Serial.println (result);
    //Serial.println(time_diff);

  }
}
