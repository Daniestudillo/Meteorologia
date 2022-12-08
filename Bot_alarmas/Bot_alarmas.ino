//Bot
#include "CTBot.h" //se incluye la biblioteca del bot
CTBot myBot; //se le declara un nombre para usar el bot

//Leds
#define ledT 23
#define ledH 26
#define ledP 25

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

//Variables
float t; //variable para leer la temperatura
float h; //variable para leer la humedad
float t2; //variable para leer la temperatura del BME280
float h2; //variable para leer la humedad del BME280
float p; //variable para leer la presión atmosférica
float a; //variable para leer la el valor aproximado de la altitud


//conexión wifi
String ssid = "Lab_Electro8283";
String pass = "0123456789";
String token = "5669097714:AAFamh2ciL2AXekrrQ_MOvhosexA6KV6i9o"; //tokken del bot


//FUNCIÓN SETUP
void setup() {
  //se inicia la comunicación serial
  Serial.begin(115200);
  Serial.println("Comenzando TelegramBot...");
  
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

  //Bot
  myBot.wifiConnect(ssid, pass); //conexión wifi
  myBot.setTelegramToken(token); //tokken del bot 
  delay(2000); //tiempo de espera de 2 segundos

  //comprobar si el microcontrolador está conectado con el bot
  if (myBot.testConnection())
    Serial.println("Conectado con Bot");
  else
    Serial.println("Error en la conexión");
}


//FUNCIÓN LOOP
void loop(){
  TBMessage msg; //se declara msg para poder obtener los mensajes desde el bot
      t = dht.readTemperature(); //temperatura leída del DHT11
      t2 = bme.readTemperature(); //temperatura leída del BME280
      h = dht.readHumidity(); //humedad leída del DHT11
      h2 = bme.readHumidity(); //humedad leída del BME280
      p = bme.readPressure()/100.0F; //presión leída         

   if(t<0 || t>50){
    digitalWrite(ledT,HIGH); //se prende el ledT
    }

   if(t>0 && t<50){
    digitalWrite(ledT,LOW); //se apaga el ledT
    }
    
   if(h<20 || h>90){
    digitalWrite(ledH,HIGH); //se prende el ledH
    }

   if(h>20 && h<90){
    digitalWrite(ledH,LOW); //se apaga el ledH
    }

   if(p<300 || p>1100){
    digitalWrite(ledP,HIGH); //se prende el ledP
    }

   if(p>300 && p<1100){
    digitalWrite(ledP,LOW); //se apaga el ledP
    }

  //cuando se obtiene un mensaje
  if (myBot.getNewMessage(msg)) {
    //inicializa la comunicación con el bot
    if (msg.text.equalsIgnoreCase("/start")){
      Serial.print("Nueva interacción de: ");
      Serial.println(msg.sender.username);
      myBot.sendMessage(msg.sender.id, "Hola, por favor, elige un de las siguientes opciones:");
      myBot.sendMessage(msg.sender.id, " 1) Envía /tem si quieres saber la temperatura actual.");
      myBot.sendMessage(msg.sender.id, " 2) Envía /hum si quieres saber la humedad.");
      myBot.sendMessage(msg.sender.id, " 3) Envía /pres si quieres saber la presión atmosférica.");
      myBot.sendMessage(msg.sender.id, " 4) Envía /alt si quieres saber el valor aproximado de la altitud.");                  
    }     
    //si se obtiene el mensaje "/tem", se obtiene el valor de la temperatura
    else if (msg.text.equalsIgnoreCase("/tem")){
      String mensaje1 = (String)"El valor de la temperatura es: " + (String)t + " °C";
      String mensaje2 = (String)"El valor de la temperatura del BME280 es: " + (String)t2 + " °C";      
      myBot.sendMessage(msg.sender.id, mensaje1);
      myBot.sendMessage(msg.sender.id, mensaje2);              
    }
    //si se obtiene el mensaje "/hum", se obtiene el valor de la humedad
    else if (msg.text.equalsIgnoreCase("/hum")){
      String mensaje1 = (String)"El valor de la humedad es: " + (String)h + " %";
      String mensaje2 = (String)"El valor de la humedad del BME280 es: " + (String)h2 + " %";      
      myBot.sendMessage(msg.sender.id, mensaje1);
      myBot.sendMessage(msg.sender.id, mensaje2);             
    }
    //si se obtiene el mensaje "/pres", se obtiene el valor de la presión atmosférica
    else if (msg.text.equalsIgnoreCase("/pres")){       
      String mensaje1 = (String)"El valor de la presión atmosférica es: " + (String)p + " hPa";     
      myBot.sendMessage(msg.sender.id, mensaje1);            
    }
    //si se obtiene el mensaje "/alt", se obtiene el valor de la altitud
    else if (msg.text.equalsIgnoreCase("/alt")){
      a = bme.readAltitude(SEALEVELPRESSURE_HPA);       
      String mensaje1 = (String)"El valor aproximado de la altitud es: " + (String)a + " m";     
      myBot.sendMessage(msg.sender.id, mensaje1);            
    }         
    //en otro caso
    else {
      myBot.sendMessage(msg.sender.id, "Intenta de nuevo con: /start"); //pide que lo intenetes de nuevo        
  }  
  
  delay(50); //tiempo de espera de 50 milisegundos
  }
}
