//DHT11
#include "DHT.h"
#define DHTPIN 32 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//Sensor BME280
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C
unsigned long delayTime;

// Fill-in information from your Blynk Template here
#define BLYNK_TEMPLATE_ID "TMPLfLme7_az"
#define BLYNK_DEVICE_NAME "Proyecto"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

#include "BlynkEdgent.h"


//-----------------Interruptores----------------------------------
BLYNK_WRITE(V8){ //Interruptor de temperatura
  boolean status_temp = param.asInt();
  //Verificar el estado del interruptor de la temperatura
  if(status_temp==0){
    Blynk.virtualWrite(V0, 0);
    Blynk.virtualWrite(V1, LOW);     
  }
  else{
    float t = dht.readTemperature();
    Blynk.virtualWrite(V0, t);
    Blynk.virtualWrite(V1, HIGH);      
  }   
}

BLYNK_WRITE(V9){ //Interruptor de humedad
  boolean status_hum = param.asInt();
  //Verificar el estado del interruptor de la humedad
  if(status_hum==0){
    Blynk.virtualWrite(V2, 0);
    Blynk.virtualWrite(V3, LOW);     
  }
  else{
    float h = dht.readHumidity();
    Blynk.virtualWrite(V2, h);
    Blynk.virtualWrite(V3, HIGH);      
  }   
}

BLYNK_WRITE(V10){ //Interruptor de presión
  boolean status_pres = param.asInt();
  //Verificar el estado del interruptor de la presión atmosférica
  if(status_pres==0){
    Blynk.virtualWrite(V4, 0);
    Blynk.virtualWrite(V5, LOW);     
  }
  else{
    float p = bme.readPressure()/100.0F;
    Blynk.virtualWrite(V4, p);
    Blynk.virtualWrite(V5, HIGH);      
  }   
}

BLYNK_WRITE(V11){ //Interruptor de altitud
  boolean status_pres = param.asInt();
  //Verificar el estado del interruptor de la altitud
  if(status_pres==0){
    Blynk.virtualWrite(V6, 0);
    Blynk.virtualWrite(V7, LOW);     
  }
  else{
    float a = bme.readAltitude(SEALEVELPRESSURE_HPA);
    Blynk.virtualWrite(V6, a);
    Blynk.virtualWrite(V7, HIGH);      
  }   
}




//FUNCIÓN SETUP
void setup(){
  //Comunicación serial
  Serial.begin(115200);
  delay(100);

  //DHT11
  dht.begin();  

  //BME280
  while(!Serial);    // time to get serial running
  Serial.println(F("BME280 test"));
  unsigned status;
  // default settings
  status = bme.begin(0x76);  
  // You can also pass in a Wire library object like &Wire2
  // status = bme.begin(0x76, &Wire2)
  if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
  }
  Serial.println("-- Default Test --");
  delayTime = 1000;
  Serial.println();

  //Se inicia Blynk
  BlynkEdgent.begin();
}


//FUNCIÓN LOOP
void loop() {
  BlynkEdgent.run();
}
