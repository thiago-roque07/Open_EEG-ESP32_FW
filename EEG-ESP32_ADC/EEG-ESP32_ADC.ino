#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

Adafruit_ADS1115 ads;     // Use this for the 16-bit version 


const int data_length = 512;

unsigned int period = 4000; //4000 us = 4ms = 1/250 SPS
unsigned long previousMicros = 0;

const double hp_coeff = 0.987589583; // Coeficiente do filtro hp, com frequência de corte em 0.5Hz e Fs = 250Hz 

struct ADC_data {
  
    double a_atual = 0;   // amostra do ADS1115 channel 0
    double b_atual = 0;   // amostra do ADS1115 channel 1
    double c_atual = 0;   // amostra do ADS1115 channel 2

    double a_prev = 0;   // amostra bufferizada para o filtro, relativa ao canal 0
    double b_prev = 0;   // amostra bufferizada para o filtro, relativa ao canal 1
    double c_prev = 0;   // amostra bufferizada para o filtro, relativa ao canal 2

    double a_filt_atual = 0;   // Amostra filtrada, correspondente ao canal 0
    double b_filt_atual = 0;   // Amostra filtrada, correspondente ao canal 1
    double c_filt_atual = 0;   // Amostra filtrada, correspondente ao canal 2
    
    double a_filt_prev = 0;   // buffer da amostra filtrada, relativa ao canal 0
    double b_filt_prev = 0;   // buffer da amostra filtrada, relativa ao canal 1
    double c_filt_prev = 0;   // buffer da amostra filtrada, relativa ao canal 2
    
    bool new_read = 0;        // Variável para indicar nova leitura
} sample;



//TaskHandle_t Task1;
BluetoothSerial SerialBT;

void setup() {
  
  Serial.begin(115200);
  Serial.println("Hello!");
  
  SerialBT.begin("ESP32_BT"); //Bluetooth device name
  Serial.println("The device started, now you can pair it with bluetooth!");

//  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
//  xTaskCreatePinnedToCore(
//                    Task1code,   /* Task function. */
//                    "Task1",     /* name of task. */
//                    10000,       /* Stack size of task */
//                    (void*)&sample,        /* parameter of the task */
//                    0,           /* priority of the task */
//                    &Task1,      /* Task handle to keep track of created task */
//                    0);          /* pin task to core 0 */                  
  delay(500); 

  ads.setGain(GAIN_TWO);           // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  ads.begin();
  ads.setDataRate(RATE_ADS1115_250SPS);  
  
  Serial.println("Setup ready!");
}



//void Task1code( void * pvParameters ){
//  ADC_data *ptr_sample = ((ADC_data*)pvParameters);
//  
//  Serial.print("Task1 running on core ");
//  Serial.println(xPortGetCoreID());
//
//  
//  for(;;){
//    if (ptr_sample->new_read) {   
//      Serial.println(ptr_sample->a);
//      ptr_sample->new_read = false;
//    }
//    delay(1);
//  }
//  vTaskDelete( NULL );
//}

void loop() {
  unsigned long currentMicros = micros(); 
  if (currentMicros - previousMicros > period) { // interval passed?
    previousMicros = currentMicros; // save the last time

    // Bufferiza as amostras presentes para uso do filtro hp //
    sample.a_prev = sample.a_atual;
    sample.b_prev = sample.b_atual;
    sample.c_prev = sample.c_atual;
    sample.a_filt_prev = sample.a_filt_atual;

    // Lê as novas amostras //  
    sample.a_atual = (double) ads.readADC_SingleEnded(0);
    sample.b_atual = (double) ads.readADC_SingleEnded(1);
    sample.c_atual = (double) ads.readADC_SingleEnded(2);

    // Realiza a filtragem para remover o nível DC do sinal // 
//    sample.a_filt_atual = hp_coeff*(sample.a_filt_prev + sample.a_atual - sample.a_prev);
    sample.b_filt_atual = hp_coeff*(sample.b_filt_prev + sample.b_atual - sample.b_prev);
//    sample.c_filt_atual = hp_coeff*(sample.c_filt_prev + sample.c_atual - sample.c_prev);

    // Envia a amostra filtrada pela serial //
    Serial.println(sample.b_filt_atual);

    // Envia a amostra filtrada pelo Bluetooth //
//    SerialBT.println(sample.b_filt_atual);
  }
    
}
