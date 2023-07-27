//Version 27-07-2023
#include <Arduino.h>
#include <ESP8266WiFi.h>

//Project specific librarys
#include <PubSubClient.h>
//#include <ArduinoJson.h>
#include <NeoPixelBus.h>

//Network & MQTT Config: Safely stored in Platformio.ini
const char* ssid = SSID_NAME; 
const char* password = SSID_PASSWORD;
//const char *host = JSON;
const char* mqtt_server = "192.168.90.29";

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


//Client declaration for used modules
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

//Module for Serial Line-write to display the messages received trough subscription, and to turn on the LED
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
//Turning on the LED if the message starts with the 't' of True.
  if ((char)payload[0] == 't')
{
  for (int i = 0; i < PixelCount; i++)
    {
      strip.SetPixelColor(i, orange);
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

//Module for reconnection if anything fails
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {

  //debug setup
  Serial.begin(9600);
  Serial.setDebugOutput(true);

  Serial.println();

  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);


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
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
