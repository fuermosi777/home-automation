#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Servo.h> 

#define DELAY_BETWEEN_COMMANDS 1000
#define LOCK_DOOR 120
#define UNLOCK_DOOR 14
#define LOCKING 0

Servo myservo;

const char* ssid = "";
const char* password = "";
const char* hname = "doorlock";
int state = LOCK_DOOR;

ESP8266WebServer server(80);

const int led = BUILTIN_LED;
const int button_pin = 5; // GPIO 5, D1. 
const int servo_pin = 4; // GPIO 4 D2.

void handleNotFound(){
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
}

void setup(void){
  myservo.attach(servo_pin);
  myservo.write(LOCK_DOOR);
  
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
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

  server.on("/lock", [](){
    lockDoor();
    server.send(200, "text/plain", "Door locked");
  });

  server.on("/unlock", [](){
    unlockDoor();
    server.send(200, "text/plain", "Door unlocked");
  });

  server.on("/state", [](){
    if (state == LOCK_DOOR) {
      server.send(200, "text/plain", "LOCK");
    } else {
      server.send(404, "text/plain", "NOT LOCK");
    }
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP Server Started.");

  // Take GPIO D1 as button.
  pinMode(button_pin, INPUT);
}

void loop(void){
  server.handleClient();

  handleButton();
}

void handleButton() {
  if (state == LOCKING) return;
  byte pressed = digitalRead(5);
  if (pressed == HIGH) {
    if (state == LOCK_DOOR) {
      unlockDoor();
    } else {
      lockDoor();
    }
  }
}

void lockDoor() {
  if (state == LOCKING) return;
  state = LOCKING;
  
  Serial.println("Locking door...");
  myservo.write(LOCK_DOOR);

  delay(1000 * 2); // After 2 seconds.
  state = LOCK_DOOR;

  digitalWrite(led, HIGH);
}

void unlockDoor() {
  if (state == LOCKING) return;
  state = LOCKING;
  
  Serial.println("Unlocking door...");
  myservo.write(UNLOCK_DOOR);

  delay(1000 * 2); // After 2 seconds.
  state = UNLOCK_DOOR;

  digitalWrite(led, LOW);
}