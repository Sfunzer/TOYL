//Version 15-04-2023
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

//Project specific librarys
#include <ArduinoJson.h>
#include <NeoPixelBus.h>

//Network & JSON Config: Safely stored in Platformio.ini
const char* ssid = SSID_NAME; 
const char* password = SSID_PASSWORD;
const char *host = JSON;

//LED-Config
const uint16_t PixelCount = 8; 
NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount);

//LED-Color Config with RGB values in steps of 255 points. 
RgbColor lampColor(128,128,128);
RgbColor lampColorOff(0,0,0);

//Colors based on front-end development
RgbColor yellow(255,226,4);
RgbColor orange(236,104,10);
RgbColor red(229,44,34);
RgbColor violet(204,64,144);
RgbColor purple(109,69,149);
RgbColor blue(28,77,155);
RgbColor marine(29,164,222);
RgbColor green(93,182,116);
RgbColor grass(162,198,45);
RgbColor lime(224,218,1);

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
DynamicJsonDocument doc(120);
deserializeJson(doc, payload);

String lamp_id = doc[0]["id"];
String lamp_state = doc[0]["lamp_on"];
String json_color = doc[0]["color"];

Serial.println("This is the ID: '"+lamp_id + "' And this is the 'ON'-State: "+lamp_state+ "' the lamp color is: '" +json_color);


//if (json_color == "red")
//{
//  lampColor == red;
//} if (json_color == "blue")
//{
//  lampColor == blue;
//}


if (lamp_state == "true")
{
  for (int i = 0; i < PixelCount; i++)
    {
      strip.SetPixelColor(i, marine);
    }
    strip.Show();

  Serial.println("Light is 'ON'");
} else {
  for (int i = 0; i < PixelCount; i++)
    {
      strip.SetPixelColor(i, lampColorOff);
    }
    strip.Show();

  Serial.println("Light is 'OFF'");
}
delay(0);
}
