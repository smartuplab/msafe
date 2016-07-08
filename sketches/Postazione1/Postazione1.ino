#include <YunClient.h>
#include <SPI.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <dht.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "MutichannelGasSensor.h"

#define humiture_pin A1 //collegare il sensore DHT11 o simile al pin A1 dello yun
#define MQTT_SERVER "192.168.0.50"
#define lux_pin A0  //collegare il sensore lux 1143 o simili al pin A0
#define sound_pin A2 //collegare il sensore suono 1133 o simili al pin A2

dht DHT;
YunClient yun;
SoftwareSerial mySerial(10,11); //rxout su pin 11 txout su pin10

float ammoniaca;
float MonDiCarb;
float BioDiAz;
double lum;
double e=2.7182818284590452354;
double humidity;
double temp;
double sound;
uint8_t i;
int16_t co2ppm;
uint8_t temperature;
//Esadecimale per misurare il CO2 e la temperatura
uint8_t measure[] =   {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79};
//Esadecimale per avviare la procedura di calibrazione
uint8_t calibrate[] = {0xFF,0x87,0x87,0x00,0x00,0x00,0x00,0x00,0xF2};
   
//Definizione del metodo di trasmissione dati
PubSubClient mqtt(MQTT_SERVER, 1883, callback, yun);
//funzione per la trasmissione dati da arduino a server
void callback(char* topic, byte* payload, unsigned int length) {
  
}

void setup()
{
  //Serial.begin(57600);
  //while (!Serial) {
    ; // Necessario solo se si vuole visualizzare qualcosa sulla seriale
  //}
  pinMode(10,INPUT);
  pinMode(11,OUTPUT);
  mutichannelGasSensor.begin(0x04);
  //Aspetto che il sensore sia pronto - Il datasheet dice ha ha bisogno di 3 minuti per essere pronto. 
  //Serial.println("Startup-Wait3Min");
 delay(180000);
  //Avvio la seriale con il sensore
  mySerial.begin(9600);
  //Serial.println("Connecting...");
  Bridge.begin();
  sendcommand(calibrate);
  mutichannelGasSensor.doCalibrate();
 delay(180000);
 mutichannelGasSensor.powerOn();
}

void loop()
{
  if (mqtt.connect("YUNClient", "test", "test")) { //tentativo di connessione al server

    //reading humidity and temperature
    DHT.read11(humiture_pin);
    humidity = DHT.humidity;
    temp = DHT.temperature;
    char charHUM[10];
    String h = String(humidity);
    h.toCharArray(charHUM, 10);
    mqtt.publish("hum1", charHUM);
    delay(1000);
    char charTEMP[10];
    String t= String(temp);
    t.toCharArray(charTEMP,10);
    mqtt.publish("temp1",charTEMP);
    delay(1000);
    
    //reading lux (sensor 1143)
    lum= analogRead(lux_pin);
    lum=lum*0.02352-0.3896;
    lum=pow(e,lum);
    String l=String(lum);
    char charLux[10];
    l.toCharArray(charLux,10);
    mqtt.publish("lum1",charLux);
    delay(1000);
    
    //reading sound (sensor 1133)
    sound= log (analogRead(sound_pin));
    sound=sound*16.801+9.872;
    String s=String(sound);
    char charSound[10];
    s.toCharArray(charSound,10);
    mqtt.publish("sound1",charSound);
    delay(1000);

    //reading CO2
    sendcommand(measure);
    if (getresult(&co2ppm,&temperature)) {
      String c= String (co2ppm);
      char charCo2[10];
      c.toCharArray(charCo2,10);
      mqtt.publish("co2-1",charCo2);

    } else {
      //Probabilmente è i cavi solo collegati male. Lo comunico sul serial monitor
      Serial.println("Double check wiring");
    }
    delay(1000);  

    //reading NH3
    ammoniaca= mutichannelGasSensor.measure_NH3();
    String ammonia =String(ammoniaca);
    char charNH3[10];
    ammonia.toCharArray(charNH3,10);
    mqtt.publish("nh3-1",charNH3);
    delay(1000);
    
    //reading NO2
    BioDiAz = mutichannelGasSensor.measure_NO2();
    String nox =String(BioDiAz);
    char charNO2[10];
    nox.toCharArray(charNO2,10);
    mqtt.publish("no2-1",charNO2);
    delay(1000);
    
    //reading CO
    MonDiCarb = mutichannelGasSensor.measure_CO();
    String carb_mon =String(MonDiCarb);
    char charCO[10];
    carb_mon.toCharArray(charCO,10);
    mqtt.publish("co-1",charCO);
    delay(1000);
  }
  delay(4000);              
}

//Metodo per inviare il comando al sensore
void sendcommand(uint8_t *pcommand)
{
    uint8_t i;
   
    for (i=0;i<9;i++) {
        mySerial.write(pcommand[i]);
        
    }
    
}

//Metodo per farsi restituire i valori dal sensore
uint8_t getresult(int16_t *pppm, uint8_t *ptemp)
{
    uint8_t i;
    uint8_t buffer[9];
    
    for (i=0;i<9;i++) {
        if ( mySerial.available() ) {
            buffer[i] = mySerial.read();
        } else {
          //Mostro l'errore sul serial monitor
          Serial.println("error 1");
          Serial.print(i);
          delay(1500);
            return 0;
        }
    }
    
    
    if (buffer[0] != 0xff) {
      //Mostro l'errore sul serial monitor
      Serial.print("error 2");
      Serial.print(i);
      Serial.print(buffer[0]);
      delay(1500);
      return 0;
    }
    
    if (buffer[1] != 0x86) {
      //Mostro l'errore sul serial monitor
      Serial.print("error 3");
      Serial.print(i);
      Serial.print(buffer[1]);
      delay(1500);
      return 0;
    }
    
    *pppm = buffer[3];
    *pppm |= ((int16_t)buffer[2] << 8);
  
    *ptemp = buffer[4] -40;  
    return 1;
}
