#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRsend.h>

#define DELAY_BETWEEN_COMMANDS 1000

IRsend irsend(15); // GPIO 15

const char* ssid = "";
const char* password = "";
const char* hname = "";

ESP8266WebServer server(80);

const int led = BUILTIN_LED;

// AC power on/off
uint16_t acPowerRawData[37] = {8500,4300, 550,1600, 550,1600, 550,1600, 500,600, 550,1600, 550,600, 550,1600, 550,1600, 500,4300, 550,1600, 500,1650, 550,550, 550,1600, 550,550, 550,550, 550,550, 550,550, 600};  // UNKNOWN B5E9B811
uint16_t fanPowerRawData[95] = {1350,400, 1350,400, 500,1250, 1350,400, 1300,400, 500,1250, 500,1250, 450,1250, 500,1250, 1350,400, 500,1250, 450,8000, 1350,400, 1350,400, 500,1250, 1350,400, 1350,400, 500,1250, 500,1250, 450,1250, 500,1250, 1350,400, 500,1250, 500,8000, 1300,400, 1350,400, 500,1250, 1350,400, 1300,400, 500,1250, 450,1250, 500,1250, 500,1250, 1350,400, 500,1250, 500,8000, 1350,400, 1350,400, 500,1250, 1350,400, 1350,400, 500,1250, 450,1250, 500,1250, 450,1250, 1300,400, 500,1250, 450};  // UNKNOWN EF1B843F
uint16_t fanSpeeRawData[85] = {1300,450, 450,1300, 450,1300, 450,1300, 1300,450, 1300,450, 450,8050, 1300,450, 1300,450, 450,1300, 1300,450, 1300,450, 1300,450, 450,1300, 450,1300, 450,1300, 1300,450, 1300,450, 450,8050, 1300,450, 1300,450, 450,1300, 1300,450, 1300,450, 1300,450, 450,1300, 450,1300, 450,1300, 1300,450, 1300,450, 450,8050, 1300,450, 1300,450, 450,1300, 1300,450, 1300,450, 1300,450, 450,1300, 450,1300, 450,1300, 1300,450, 1300,450, 450};  // UNKNOWN 4D704C89
uint16_t fanOscillationRawData[85] = {1300,450, 450,1300, 450,1300, 450,1300, 450,1300, 1300,450, 1300,7200, 1300,450, 1300,450, 450,1300, 1300,450, 1300,450, 1300,450, 450,1300, 450,1300, 450,1300, 450,1300, 1300,450, 1300,7200, 1300,450, 1300,450, 450,1300, 1300,450, 1300,450, 1300,450, 450,1300, 450,1300, 450,1300, 450,1300, 1300,450, 1300,7200, 1300,450, 1300,450, 450,1300, 1300,450, 1300,450, 1300,450, 450,1300, 450,1300, 450,1300, 450,1300, 1300,450, 1300};  // UNKNOWN A063E6FE


uint16_t soundbarPowerRawData[77] = {4500,4550, 550,500, 550,500, 550,500, 550,500, 550,1500, 550,1550, 550,500, 550,500, 550,1550, 550,1550, 550,1550, 550,1550, 550,500, 550,500, 550,500, 550,500, 550,4500, 550,500, 550,500, 550,500, 550,500, 550,500, 550,500, 550,500, 550,500, 550,1550, 550,1550, 550,1400, 650,500, 550,1500, 550,1550, 550,1500, 550,1500, 550,500, 550,500, 550,500, 550,1500, 550};  // UNKNOWN CA31DA45
uint16_t soundbarSourceRawData[77] = {4500,4550, 550,500, 550,500, 500,550, 500,550, 500,1550, 550,1500, 550,500, 500,550, 500,1550, 550,1500, 550,1500, 550,1500, 550,500, 550,500, 500,550, 550,500, 550,4500, 550,500, 550,500, 550,500, 550,500, 550,1500, 550,500, 550,1500, 550,500, 550,1500, 550,1500, 550,1500, 550,500, 550,500, 550,1500, 550,500, 550,1500, 550,500, 550,500, 550,500, 550,1500, 550};  // UNKNOWN 17112D07

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
  WiFi.hostname(hname);
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

  if (MDNS.begin(hname)) {
    Serial.println("MDNS Responder Started");
  }

  server.on("/ac_power", [](){
    Serial.println("A/C ON/OFF");
    irsend.sendRaw(acPowerRawData, 37, 38);
    server.send(200, "text/plain", "A/C ON/OFF");
  });

  server.on("/fan_power", [](){
    Serial.println("Fan ON/OFF");
    irsend.sendRaw(fanPowerRawData, 95, 38);
    server.send(200, "text/plain", "Fan ON/OFF");
  });

  server.on("/fan_oscillation", [](){
    Serial.println("Fan oscillation ON/OFF");
    irsend.sendRaw(fanOscillationRawData, 85, 38);
    server.send(200, "text/plain", "Fan oscillation ON/OFF");
  });

  server.on("/soundbar_power", [](){
    Serial.println("Soundbar power ON/OFF");
    irsend.sendRaw(soundbarPowerRawData, 77, 38);
    server.send(200, "text/plain", "Soundbar power ON/OFF");
  });

  server.on("/soundbar_source", [](){
    Serial.println("Soundbar source ON/OFF");
    irsend.sendRaw(soundbarSourceRawData, 77, 38);
    server.send(200, "text/plain", "Soundbar source ON/OFF");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP Server Started");
}

void loop(void){
  server.handleClient();
}