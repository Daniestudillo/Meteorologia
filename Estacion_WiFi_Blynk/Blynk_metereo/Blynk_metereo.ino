/*************************************************************

  Script for the Blynk Metereological station app developed at 
  the  Electronics Lab of Facultad de Ciencias UNAM integrated
  with an Espressif ESP-WROOM-32 microcontroller that performs
  switching on/off sensor readings, sends and plots temperature,
  humidity, pressure and altitude. Also, checks for sensor limit
  values and user limit values using an alarm likewise system, all via 
  WIFI conection.

  Default sensors used are DHT11 and BME280/BMP280. For different
  components adjust the sensor reading code and limits.

  Script developed by:

  Luis Muñoz Flores
  Facultad de Ciencias/ Instituto de Ciencias Aplicadas y Tecnologia
  Universidad Nacional Autonoma de Mexico
  <luis.munoz_flores@ciencias.unam.mx> 2022

  Modified by:

  Daniel Montana Estudillo Hernandez
  Facultad de Ciencias
  Universidad Nacional Autonoma de Mexico
  <daniestudillo@ciencias.unam.mx> 2022

 *************************************************************/

//DHT11
#include "DHT.h"
#define DHTPIN 32 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//Sensor BME280
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#define BMP_SCK 13
#define BMP_MISO 12
#define BMP_MOSI 11
#define BMP_CS 10
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BMP280 bmp; // I2C
//unsigned long delayTime;


// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "XXXXXXX"
#define BLYNK_DEVICE_NAME "XXXXXXX"
#define BLYNK_AUTH_TOKEN "XXXXXXX"


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;

// Your WiFi credentials.
// Set password to "" for open networks.

//char ssid[] = "XXXXXXX";
//char pass[] = "";

BlynkTimer reading, limiter;

boolean status_temp, status_hum, status_pres, status_alt;
double t, h, p, a;
boolean lim_temp, lim_hum, lim_pres;
double max_temp, max_hum, min_temp, min_hum, max_pres, min_pres;
int t_ledPin = 33, h_ledPin = 25, p_ledPin = 26, buzzer_pin = 23;
boolean alarmActive=false;
int alarmDelay=0;

//***********************END OF DECLARATION***********************

//----------------------------Switches----------------------------
BLYNK_WRITE(V8){ // Temperature switch change

  status_temp = param.asInt();
  //Verificar el estado del interruptor de la temperatura
  if(status_temp==0){
    Blynk.virtualWrite(V0, 0); // Sensor off
    Blynk.virtualWrite(V1, LOW); // Virtual Led    
    digitalWrite(t_ledPin, LOW);
    digitalWrite(buzzer_pin, LOW);
  }
  else{
    t = dht.readTemperature();
    Blynk.virtualWrite(V0, t); // Sensor
    Blynk.virtualWrite(V1, HIGH); // Virtual Led   
    digitalWrite(t_ledPin, HIGH);  
  }   
}

BLYNK_WRITE(V9){ // Humidity switch change
  status_hum = param.asInt();
  //Verificar el estado del interruptor de la humedad
  if(status_hum==0){
    Blynk.virtualWrite(V2, 0); // Sensor off
    Blynk.virtualWrite(V3, LOW); // Virtual Led   
    digitalWrite(h_ledPin, LOW); 
  }
  else{
    h = dht.readHumidity();
    Blynk.virtualWrite(V2, h); // Sensor
    Blynk.virtualWrite(V3, HIGH); //Virtual Led   
    digitalWrite(h_ledPin, HIGH);  
  }   
}

BLYNK_WRITE(V10){ //Pressure switch change
  status_pres = param.asInt();
  //Verificar el estado del interruptor de la presión atmosférica
  if(status_pres==0){
    Blynk.virtualWrite(V4, 0);  // Sensor off
    Blynk.virtualWrite(V5, LOW); // Virtual Led 
    digitalWrite(p_ledPin, LOW);   
  }
  else{
    p = bmp.readPressure()/100.0D;
    Blynk.virtualWrite(V4, p); // Sensor
    Blynk.virtualWrite(V5, HIGH); // Virtual Led   
    digitalWrite(p_ledPin, HIGH);  
  }   
}

BLYNK_WRITE(V11){ //Altitude switch change
  status_alt = param.asInt();
  //Verificar el estado del interruptor de la altitud
  if(status_pres==0){
    Blynk.virtualWrite(V6, 0); // Sensor off
    Blynk.virtualWrite(V7, LOW); // Virtual Led    
  }
  else{
    a = bmp.readAltitude(SEALEVELPRESSURE_HPA);
    Blynk.virtualWrite(V6, a);  // Sensor on
    Blynk.virtualWrite(V7, HIGH); // Virtual Led     
  }   
}


// This function is called every time the device is connected to the Blynk.Cloud

BLYNK_CONNECTED()
{
  
  // Sync actual application values as system gets reconected
  Blynk.syncAll();

  digitalWrite(buzzer_pin, HIGH);  
  delay(250);
  digitalWrite(buzzer_pin, LOW);  
  // Sound Buzzer if connected
  // Frecuencia, duracionOn, duracionOff, beeps, duracionPausa, secuencia, funcionCallback
  //EasyBuzzer.singleBeep(440, 500);

  // Sync switch values to start reading sensors
  // Blynk.syncVirtual(V8,V9,V10,V11);

}


// This function sends sensor readings every half-second to the app.
void readSensor()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.

  //Read temperature sensor in loop

  if(status_temp==0){
    Blynk.virtualWrite(V0, 0);
    Blynk.virtualWrite(V1, LOW);  //Turn off temperature app led  
    digitalWrite(t_ledPin, LOW);  //Turn off temperature physical led   
  }
  else{
    t = dht.readTemperature();
    Blynk.virtualWrite(V0, t);
    Blynk.virtualWrite(V1, HIGH);   //Turn on temperature app led 
    digitalWrite(t_ledPin, HIGH);   //Turn on temperature physical led
  }   

  //Read humidity sensor in loop

  if(status_hum==0){
    Blynk.virtualWrite(V2, 0); // Sensor off
    Blynk.virtualWrite(V3, LOW); // Virtual Led 
    digitalWrite(h_ledPin, LOW);   
  }
  else{
    h = dht.readHumidity();
    Blynk.virtualWrite(V2, h); // Sensor
    Blynk.virtualWrite(V3, HIGH); //Virtual Led    
    digitalWrite(h_ledPin, HIGH); 
  }

  // Read pressure sensor in loop

  if(status_pres==0){
    Blynk.virtualWrite(V4, 0);  // Sensor off
    Blynk.virtualWrite(V5, LOW); // Virtual Led   
    digitalWrite(p_ledPin, LOW); 
  }
  else{
    p = bmp.readPressure()/100.0F;
    Blynk.virtualWrite(V4, p); // Sensor
    Blynk.virtualWrite(V5, HIGH); // Virtual Led 
    digitalWrite(p_ledPin, HIGH);    
  }   

  // Read altitude sensor in loop

  if(status_alt==0){
    Blynk.virtualWrite(V6, 0); // Sensor off
    Blynk.virtualWrite(V7, LOW); // Virtual Led    
  }
  else{
    a = bmp.readAltitude(SEALEVELPRESSURE_HPA);
    Blynk.virtualWrite(V6, a);  // Sensor on
    Blynk.virtualWrite(V7, HIGH); // Virtual Led     
  }

}

//***************SWITCHES DEFINITION****************

BLYNK_WRITE(V1) //Interruptor de temperatura
{
  status_temp = param.asInt();
}

BLYNK_WRITE(V0) //Interruptor de humedad
{
  status_hum = param.asInt();
}

BLYNK_WRITE(V14) //Interruptor de maximos y mínimos de temperatura
{
  lim_temp = param.asInt();
}

BLYNK_WRITE(V19) //Interruptor de maximos y mínimos de humedad
{
  lim_hum = param.asInt();
}

BLYNK_WRITE(V23) //Interruptor de maximos y mínimos de presion
{
  lim_pres = param.asInt();
}

//*************************************************************

//************MINIMUM AND MAXIMUM DEFINITION VALUES************

BLYNK_WRITE(V13) //Temperature minimum
{
  min_temp = param.asDouble();
}

BLYNK_WRITE(V12) //Temperature maximum
{
  max_temp = param.asDouble();
}

BLYNK_WRITE(V17) //Humidity minimum
{
  min_hum = param.asDouble();
}

BLYNK_WRITE(V16) //Humidity maximum
{
  max_hum = param.asDouble();
}

BLYNK_WRITE(V22) //Pressure minimum
{
  min_pres = param.asDouble();
}

BLYNK_WRITE(V21) //Pressure maximum
{
  max_pres = param.asDouble();
}

//***********************BLINKING LEDS ALARM***********************

void ledblink_t() //Set the alarm physical and virtual led's blinking
{
  // Frecuencia, duracionOn, duracionOff, beeps, duracionPausa, secuencia, funcionCallback

  for(int iCounter = 0; iCounter<=4; iCounter++){
    digitalWrite(t_ledPin, HIGH);  
    Blynk.virtualWrite(V15, HIGH); // Virtual Led
    delay(500);
    digitalWrite(t_ledPin, LOW);  
    Blynk.virtualWrite(V15, LOW); // Virtual Led
    delay(500);
  }
}

void alarm(){
  if (alarmActive==false){
    for(int iCounter = 0; iCounter<=4; iCounter++){
      digitalWrite(buzzer_pin, HIGH);
      delay(300);
      digitalWrite(buzzer_pin, LOW);
      delay(700);
    }
  }
}

void ledblink_h()
{
  for(int iCounter = 0; iCounter<=4; iCounter++){
    digitalWrite(h_ledPin, HIGH);
    Blynk.virtualWrite(V26, HIGH);
    delay(500);
    digitalWrite(h_ledPin, LOW);
    Blynk.virtualWrite(V26, LOW);
    delay(500);
  }
}

void ledblink_p()
{
  for(int iCounter = 0; iCounter<=4; iCounter++){
    digitalWrite(p_ledPin, HIGH);    
    Blynk.virtualWrite(V25, HIGH);
    delay(500);
    digitalWrite(p_ledPin, LOW);
    Blynk.virtualWrite(V25, LOW);
    delay(500);
  }
}

//****************************************************************

void limits() //Define actions if stablished limits are exceeded
{
  //############LIMITS OF SENSOR DHT T:(0°C a 50°C), H:(20% a 90%))############

  if(status_temp==1){
    if(t<0 || t>50){ //Si excede los límites de temperatura, enciende alarma
      Blynk.virtualWrite(V15, HIGH); // Virtual Led  
      alarm();
      alarmActive=true;
      ledblink_t(); //Encender la alarma del led real
    }
  }

  if(status_hum==1){
    if(h<20 || h>90){ //Si excede los límites de humedad, enciende alarma
      Blynk.virtualWrite(V3, HIGH); // Virtual Led 
      alarm();
      alarmActive=true;
      ledblink_h(); //Encender la alarma del led real
    }
  }
  
  //*************************************************

  //####LIMITS OF SENSOR BME280 T:(-40°C a 85°C), P:(300 hpa a 1100 hpa) ######

  if(status_temp==1){
    if(t<-40 || t>84){ //Si excede los límites de temperatura, enciende alarma
      Blynk.virtualWrite(V15, HIGH); // Virtual Led    
    }
  }

  if(status_pres==1){
    if(p<300 || p>1100){ //Si excede los límites de presión, enciende alarma
      Blynk.virtualWrite(V10, LOW); // Virtual Led  
      alarm();
      alarmActive=true;
      ledblink_p(); //Encender la alarma del led real
    }
  }

 //***************USER LIMITS***************
 
 // TEMPERATURE
 if(lim_temp == 1 && status_temp == 1){
  if(t<=min_temp || t>=max_temp){
    alarm();
    alarmActive=true;
    ledblink_t(); //Encender la alarma del led real
    Blynk.virtualWrite(V15, HIGH); //Turn on app's led
  }else{
    Blynk.virtualWrite(V15, LOW);
  }
 }

 // Mantain a minimum interval of 1 for limits w/ min<max for temperature sliders
 if(min_temp>=max_temp)
 {
   Blynk.virtualWrite(V13, max_temp-1);
 }


 // HUMIDITY
 if(lim_hum == 1 && status_hum == 1){
  if(h<=min_hum || h>=max_hum)
  {
    alarm();
    alarmActive=true;
    ledblink_h(); //Encender la alarma del led real
    Blynk.virtualWrite(V26, HIGH); //Turn on app's led
  }else{
    Blynk.virtualWrite(V26, LOW);
  }
 }

 // Mantain a minimum interval of 1 for limits w/ min<max for humidity sliders
 if(min_hum>=max_hum)
 {
   Blynk.virtualWrite(V17, max_hum-1);
 }

 // PRESSURE
 if(lim_pres == 1 && status_pres == 1){
  if(p<=min_pres || p>=max_pres)
  {
    alarm();
    alarmActive=true;
    ledblink_p(); //Encender la alarma del led real
    Blynk.virtualWrite(V25, HIGH); //Turn on app's led
  }else{
    Blynk.virtualWrite(V25, LOW);
  }
 }

 // Mantain a minimum interval of 1 for limits w/ min<max for pressure sliders
 if(min_pres>=max_pres)
 {
   Blynk.virtualWrite(V22, max_pres-1);
 }

}

void setup()
{

  pinMode(t_ledPin, OUTPUT);
  pinMode(h_ledPin, OUTPUT);
  pinMode(p_ledPin, OUTPUT);
  pinMode(buzzer_pin, OUTPUT);

  // Debug console
  //Serial.begin(115200);
  
  //DHT11
  dht.begin();  
  
  //BME280
   bmp.begin(0x76);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  reading.setInterval(500L, readSensor);
  limiter.setInterval(150L, limits);
}

void loop()
{
  Blynk.run();
  reading.run();
  limiter.run();
  alarmActive=false;
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
}

