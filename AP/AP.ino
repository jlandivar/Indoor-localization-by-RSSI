#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#define AP_NUMBER 2
#define CHANNEL 11

uint8_t number = AP_NUMBER;

void setup() {
  Serial.begin(115200);
  
  Serial.println("Configuring access point...");
  WiFi.softAP("AP" + (String)number, "", CHANNEL, false, 4);
  Serial.print("SSID: ");
  Serial.println("AP" + (String)number);
}

void loop() {
  
}
