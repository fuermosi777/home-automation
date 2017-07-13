#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Servo.h> 

#define DELAY_BETWEEN_COMMANDS 1000
#define LOCK_DOOR 24
#define UNLOCK_DOOR 134

Servo myservo;

const char* ssid = "";
const char* password = "";
const char* hname = "lock";
int state = LOCK_DOOR;

ESP8266WebServer server(80);

const int led = BUILTIN_LED;

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
  myservo.attach(4); // GPIO 4 D2
  myservo.write(LOCK_DOOR);
  
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

  server.on("/door_lock", [](){
    Serial.println("Lock door");
    myservo.write(LOCK_DOOR);
    state = LOCK_DOOR;
    server.send(200, "text/plain", "Door locked");
  });

  server.on("/door_unlock", [](){
    Serial.println("Unlock door");
    myservo.write(UNLOCK_DOOR);
    state = UNLOCK_DOOR;
    server.send(200, "text/plain", "Door unlocked");
  });

  server.on("/door_state", [](){
    Serial.println("Door state");
    if (state == LOCK_DOOR) {
      server.send(200, "text/plain", "YES");
    } else {
      server.send(200, "text/plain", "NO");
    }
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP Server Started");
}

void loop(void){
  server.handleClient();
}
