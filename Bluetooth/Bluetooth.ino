//Bluetooth
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial BT;

//Sensor DHT11
#include "DHT.h" //se incluye librería DHT
#define DHTPIN 32 // se define el sensor en el pin 32
#define DHTTYPE DHT11 //el sensor es un DHT11
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


//FUNCIÓN DEL BLUETOOTH
void callback_function(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  if (event == ESP_SPP_START_EVT) {
    Serial.println("Inicializado SPP");
  }
  else if (event == ESP_SPP_SRV_OPEN_EVT ) {
    Serial.println("Cliente conectado");
    BT.println("Para conocer el valor de la temperatura, escribe 1.");
    BT.println("Para conocer el valor de la humedad, escribe 2.");
    BT.println("Para conocer el valor de la presión atmosférica, escribe 3.");
    BT.println("Para conocer el valor aproximado de la altitud, escribe 4.");                    
  }
  else if (event == ESP_SPP_CLOSE_EVT  ) {
    Serial.println("Cliente desconectado");
  }
  else if (event == ESP_SPP_DATA_IND_EVT ) {
    Serial.println("Datos recibidos");
    while (BT.available()) { // Mientras haya datos por recibir
      float h = dht.readHumidity(); //Mide la humedad
      float t = dht.readTemperature(); //Mide la temperatura en grados Celcius
      float f = dht.readTemperature(true); //Mide la temperatura en grados Fahrenheit
      float t2 = bme.readTemperature(); //Mide la temperatura del BME280
      float h2 = bme.readHumidity(); //Mide la humedad del BME280
      float p = bme.readPressure()/100.0F; //Mide la presión atmosférica del BME280
      float a = bme.readAltitude(SEALEVELPRESSURE_HPA); //Mide la altitud aproximada
      int incoming = BT.read(); // Lee un byte de los datos recibidos
      Serial.print("Recibido: ");
      Serial.println(incoming);
      if (incoming == 49) { // 1 en ASCII, mide la temperatura
     BT.print("La temperatura en grados Celsius es de: ");
     BT.print(t);
     BT.print(" °C");
     BT.println(", o bien");
     BT.print("En grados Fahrenheit es de: ");
     BT.print(f);
     BT.println(" °F");
     BT.print("La temperatura en grados Celsius del BME280 es de: ");
     BT.print(t2);
     BT.print(" °C");     
     BT.println("");
     BT.println("");     
     delay(1000); //tiempo de espera de un segundo        
      }
      else if (incoming == 50) { // 2 en ASCII, mide la humedad
     BT.print("La humedad es de: ");
     BT.print(h);     
     BT.println(" %");     
     BT.print("La humedad del BME280 es de: ");
     BT.print(h2);     
     BT.print(" %"); 
     BT.println("");
     BT.println("");     
     delay(1000); //tiempo de espera de un segundo         
      }
      else if (incoming == 51) { // 3 en ASCII, mide la presión atmosférica
     BT.print("La presión atmosférica es de: ");
     BT.print(p);     
     BT.print(" hPa");
     BT.println("");
     BT.println("");     
     delay(1000); //tiempo de espera de un segundo         
      }
      else if (incoming == 52) { // 4 en ASCII, mide la altitud aproximada
     BT.print("La altitud aproximada es de: ");
     BT.print(a);     
     BT.print(" m");
     BT.println("");
     BT.println("");     
     delay(1000); //tiempo de espera de un segundo         
      }                   
    }
  }
}


//FUNCIÓN SETUP
void setup() {
    //DHT11
    Serial.begin(115200); //se inicia la comunicación serial
    dht.begin(); //se inicia el sensor     
    
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
        
    //Bluetooth
    Serial.begin(115200);
    BT.begin("Proyecto"); // Nombre de tu Dispositivo Bluetooth y en modo esclavo
    Serial.println("El dispositivo Bluetooth está listo para emparejar");
    BT.register_callback(callback_function);  
}


//FUNCIÓN LOOP
void loop() {
      float h = dht.readHumidity(); //Mide la humedad
      float t = dht.readTemperature(); //Mide la temperatura en grados Celcius
      float f = dht.readTemperature(true); //Mide la temperatura en grados Fahrenheit
      Serial.println(h); 
      Serial.println(t);
      Serial.println(f);
      delay(delayTime);
}
