#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// -------- Blynk Auth Token ----------
char auth[] = "Your_Blynk_Auth_Token";

// -------- WiFi Credentials ----------
char ssid[] = "Your_SSID";
char pass[] = "Your_PASSWORD";

// -------- Pin Definitions on ESP32 ----------
const int switch1Pin = 16;  // GPIO16
const int switch2Pin = 17;  // GPIO17
const int switch3Pin = 18;  // GPIO18

// Initialize switch states
bool switch1State = LOW;
bool switch2State = LOW;
bool switch3State = LOW;

// -------- Blynk Virtual Pin Handlers ----------
BLYNK_WRITE(V1) {  // Switch 1
  switch1State = param.asInt();
  digitalWrite(switch1Pin, switch1State);
}

BLYNK_WRITE(V2) {  // Switch 2
  switch2State = param.asInt();
  digitalWrite(switch2Pin, switch2State);
}

BLYNK_WRITE(V3) {  // Switch 3
  switch3State = param.asInt();
  digitalWrite(switch3Pin, switch3State);
}

void setup() {
  Serial.begin(115200);

  // Initialize switch pins
  pinMode(switch1Pin, OUTPUT);
  pinMode(switch2Pin, OUTPUT);
  pinMode(switch3Pin, OUTPUT);

  // Start Blynk connection
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
}
