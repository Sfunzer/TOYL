//Version 15-04-2023
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

//Project specific librarys
#include <ArduinoJson.h>
#include <NeoPixelBus.h>

//Network & JSON Config
const char* ssid = "RadiatorLabs";
const char* password = "L2HaH4ghz";
const char *host = "https://eihxqcwtouoobfrckoxt.supabase.co/rest/v1/lampsignal?id=eq.2&select=*&apikey=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImVpaHhxY3d0b3Vvb2JmcmNrb3h0Iiwicm9sZSI6ImFub24iLCJpYXQiOjE2NzkyNTQ0MDAsImV4cCI6MTk5NDgzMDQwMH0.m7Z667Ku3gcAnJ3y85NkWw3FMx90pfv71EVgiYedBYo";
//const char *host = "https://www.howsmyssl.com/a/check";

//LED-Config
const uint16_t PixelCount = 8; 
const uint8_t PixelPin = 2; //ignored for Esp8266
NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);

//LED-Color Config
RgbColor yellow(100,80,0);
RgbColor black(0,0,0);

void setup() {

  //debug setup
  Serial.begin(9600);
  Serial.setDebugOutput(true);

  Serial.println();

 //LED-Setup
  strip.Begin();
  strip.Show();

//Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }

 
}


void loop() {



//HTTPS Connection to JSON/SupaBase
String payload;
int httpCode;

  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    // Ignore SSL certificate validation
    client->setInsecure();
    
    //create an HTTPClient instance
    HTTPClient https;
    //Initializing an HTTPS communication using the secure client
    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, host)) {  // HTTPS
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      httpCode = https.GET();
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          payload = https.getString();
          //Serial.println(payload); //to remove in the future
        }
      } else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }
      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

Serial.println("Payload decoded:");
Serial.println(payload);

//Deserializing JSON to Strings (for now, a straight boolean needs some more work)
DynamicJsonDocument doc(60);
deserializeJson(doc, payload);

String lamp_id = doc[0]["id"];
String lamp_state = doc[0]["lamp_on"];

Serial.println("This is the ID: '"+lamp_id + "' And this is the 'ON'-State: "+lamp_state);

if (lamp_state == "true")
{
  for (int i = 0; i < PixelCount; i++)
    {
      strip.SetPixelColor(i, yellow);
    }
    strip.Show();

  Serial.println("Light is 'ON'");
} else {
  for (int i = 0; i < PixelCount; i++)
    {
      strip.SetPixelColor(i, black);
    }
    strip.Show();

  Serial.println("Light is 'OFF'");
}
delay(0);
}
