#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <time.h>

// ===============================
// WIFI CREDENTIALS
// ===============================
#define WIFI_SSID "Pogs"
#define WIFI_PASSWORD "pogipads123"

// ===============================
// FIREBASE CREDENTIALS
// ===============================
#define API_KEY "AIzaSyD1ZJIxjmJAgvqtMd-oBGpaO7vVi7LN_ks"
#define DATABASE_URL "https://earthquake-monitoring-3dc40-default-rtdb.asia-southeast1.firebasedatabase.app"

// ===============================
// FIREBASE OBJECTS
// ===============================
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ===============================
// LOGIC FUNCTIONS
// ===============================
String getLevel(float m) {
  if (m < 3.0) return "Minor";
  else if (m < 5.0) return "Light";
  else if (m < 6.0) return "Moderate";
  else if (m < 7.0) return "Strong";
  else return "Severe";
}

String getStatus(float m) {
  if (m >= 6.0) return "ALERT";
  else return "Normal";
}

// ===============================
// SETUP
// ===============================
void setup() {
  Serial.begin(115200);

  // WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Time (for history)
  configTime(0, 0, "pool.ntp.org");

  // Firebase config
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase connected");
}

// ===============================
// LOOP
// ===============================
void loop() {

  // Generate earthquake magnitude (example)
  float magnitude = random(40, 80) / 10.0;

  // Apply logic
  String level = getLevel(magnitude);
  String status = getStatus(magnitude);

  // Send to Firebase
  Firebase.RTDB.setFloat(&fbdo, "/earthquake/magnitude", magnitude);
  Firebase.RTDB.setString(&fbdo, "/earthquake/level", level);
  Firebase.RTDB.setString(&fbdo, "/earthquake/status", status);

  // Save history using timestamp
  time_t now = time(nullptr);
  Firebase.RTDB.setFloat(&fbdo,
    "/earthquake/history/" + String(now),
    magnitude
  );

  // Serial output
  Serial.println("Updated Firebase:");
  Serial.println(magnitude);
  Serial.println(level);
  Serial.println(status);
  Serial.println("--------------------");

  delay(5000); // update every 5 seconds
}
