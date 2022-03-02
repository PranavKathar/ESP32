//included libraries reuired for dht sensor
#include "DHT.h"
#include <WiFi.h>
#include <HTTPClient.h>
#define DHTPIN 4//this gpio we will use for data 
#define DHTTYPE DHT11
float h;
float t;
//wifi credentials
const char* ssid = "Pranav_pk";
const char* password =  "Aarushi!23";
//we will need these api keys for constantly updting live sensor readings
const char* serverName = "https://api.thingspeak.com/update";
String apiKey1 = "T0TALZKIBV46WH67";
String apiKey2 = "6T9GAFTTQW229AAD";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);//baud rate set to 9600
  WiFi.begin(ssid, password);//wifi initialized
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED)//constantly checking 
  {
    delay(500);
    Serial.print(".");
    }
  Serial.println("");
  Serial.print("Connected!");
  Serial.println(F("DHT11 test!"));

  dht.begin();
}

void loop() {
  if(WiFi.status() == WL_CONNECTED)
  {
    HTTPClient http;
    http.begin(serverName);
  // Wait a few seconds between measurements.
    delay(2000);
  
    h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    t = dht.readTemperature();
    
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    //publishing on thingspeak
    String HumidityDataSent = "api_key=" + apiKey1 + "&field1=" + String(h);
    String TempDataSent = "api_key=" + apiKey2 + "&field1=" + String(t);

    http.POST(HumidityDataSent);
    http.POST(TempDataSent);
  

  }
  //printing sensor readings on serial monitor
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.println("");
 
}
