#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
//#include <ESP8266mDNS.h>
#include <Wire.h>
#include <PolledTimeout.h>

#define SDA_PIN 0
#define SCL_PIN 2
const int16_t I2C_SLAVE = 0x08;
const int16_t I2C_MASTER = 0x42;

#ifndef STASSID
#define STASSID "NETLIFE-ARRIBA"
#define STAPSK  "lan_0194G"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);
String textToServer = "";

void handleRoot() {
  server.send(200, "text/plain", textToServer);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Serial.println(ssid); Serial.println(password);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  /*if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }*/

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  Wire.begin(SDA_PIN, SCL_PIN, I2C_MASTER); 
  Serial.println("Wire began");  
}

void loop(void) {
  Wire.requestFrom(I2C_SLAVE, 3);    // request 3 bytes from slave device #8
  if (Wire.available()){
    String textAcum = "";
    while (Wire.available()) { // slave may send less than requested
      textAcum += Wire.read(); // receive a byte as character
      textAcum += ",";
    }
    textToServer = textAcum;
  }
  
  server.handleClient();
  //MDNS.update();
}
