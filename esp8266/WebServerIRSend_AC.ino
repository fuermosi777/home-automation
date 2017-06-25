#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRsend.h>

#define DELAY_BETWEEN_COMMANDS 1000

IRsend irsend(15);

const char* ssid = "";
const char* password = "";

ESP8266WebServer server(80);

const int led = BUILTIN_LED;

// AC power on/off
uint16_t  acPowerRawData[37] = {8500,4300, 550,1600, 550,1600, 550,1600, 500,600, 550,1600, 550,600, 550,1600, 550,1600, 500,4300, 550,1600, 500,1650, 550,550, 550,1600, 550,550, 550,550, 550,550, 550,550, 600};  // UNKNOWN B5E9B811

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 1);
}

void setup(void){
  irsend.begin();
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  WiFi.hostname("ac");
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

  if (MDNS.begin("ac")) {
    Serial.println("MDNS Responder Started");
  }

  server.on("/ac_power", [](){
    Serial.println("A/C ON/OFF");
    irsend.sendRaw(acPowerRawData, 37, 38);
    server.send(200, "text/plain", "A/C ON/OFF");
  });

//  server.on("/chromecast", [](){
//    Serial.println("Chromecast");
//    irsend.sendNEC(0x20DFD02F, 32);
//    delay(DELAY_BETWEEN_COMMANDS);
//    irsend.sendNEC(0x20DFD02F, 32);
//    delay(DELAY_BETWEEN_COMMANDS);
//    irsend.sendNEC(0x4BB6906F, 32);
//    server.send(200, "text/plain", "Chromecast");
//  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP Server Started");
}

void loop(void){
  server.handleClient();
}