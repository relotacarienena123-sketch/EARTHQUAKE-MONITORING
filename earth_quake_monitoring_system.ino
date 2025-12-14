#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <time.h>

// ================= WIFI =================
#define WIFI_SSID "tonton"
#define WIFI_PASSWORD "relota123"

// =============== FIREBASE ===============
#define API_KEY "AIzaSyD1ZJIxjmJAgvqtMd-oBGpaO7vVi7LN_ks"
#define DATABASE_URL "https://earthquake-monitoring-3dc40-default-rtdb.asia-southeast1.firebasedatabase.app"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// ================= SETUP =================
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

  // Time for history key
  configTime(0, 0, "pool.ntp.org");

  // Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Firebase connected");
}

// ================= LOOP =================
void loop() {
  float magnitude = random(40, 80) / 10.0;  // simulate 4.0 – 8.0
  String level;
  String status = "Normal";

  // Level logic
  if (magnitude < 4.0) level = "Low";
  else if (magnitude < 6.0) level = "Moderate";
  else {
    level = "High";
    status = "Alert";
  }

  // Update live values
  Firebase.RTDB.setFloat(&fbdo, "earthquake/magnitude", magnitude);
  Firebase.RTDB.setString(&fbdo, "earthquake/level", level);
  Firebase.RTDB.setString(&fbdo, "earthquake/status", status);

  // Push history
  time_t now;
  time(&now);
  String path = "earthquake/history/" + String((long)now);
  Firebase.RTDB.setFloat(&fbdo, path, magnitude);

  Serial.println("Updated Firebase:");
  Serial.println(magnitude);

  delay(5000);  // every 5 seconds
}
