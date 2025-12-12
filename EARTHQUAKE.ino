#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Insert your WiFi credentials
#define WIFI_SSID "v27e"
#define WIFI_PASSWORD "123456789"

// Firebase project credentials
#define API_KEY "AIzaSyD1ZJIxjmJAgvqtMd-oBGpaO7vVi7LN_ks"
#define DATABASE_URL  "https://earthquake-monitoring-3dc40-default-rtdb.asia-southeast1.firebasedatabase.app"
// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Sensor pins
#define SW420_PIN 34
#define SW520_PIN 35

// Variables
int sw420Value = 0;
int sw520Value = 0;

// ---- Function to calculate magnitude ----
float calculateMagnitude(int val1, int val2) {
  int combined = (val1 + val2) / 2; // Average vibration strength

  // Convert analog intensity to a pseudo magnitude (0–8 scale)
  float magnitude = (combined / 4095.0) * 8.0;

  return magnitude;
}

// ---- Function to determine level ----
String getEarthquakeLevel(float magnitude) {
  if (magnitude < 2.0) return "Low";
  else if (magnitude < 4.0) return "Moderate";
  else if (magnitude < 6.0) return "High";
  else return "Severe";
}

void setup() {
  Serial.begin(115200);

  pinMode(SW420_PIN, INPUT);
  pinMode(SW520_PIN, INPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase login successful");
  } else {
    Serial.printf("Firebase signup error: %s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Read sensors
  sw420Value = analogRead(SW420_PIN);
  sw520Value = analogRead(SW520_PIN);

  // Compute magnitude & level
  float magnitude = calculateMagnitude(sw420Value, sw520Value);
  String level = getEarthquakeLevel(magnitude);

  // Print to Serial
  Serial.print("SW420: ");
  Serial.print(sw420Value);
  Serial.print(" | SW520: ");
  Serial.print(sw520Value);
  Serial.print(" | Magnitude: ");
  Serial.print(magnitude, 2);
  Serial.print(" | Level: ");
  Serial.println(level);

  // Upload to Firebase
  if (Firebase.ready()) {
    Firebase.RTDB.setInt(&fbdo, "/Earthquake/SW420_Value", sw420Value);
    Firebase.RTDB.setInt(&fbdo, "/Earthquake/SW520_Value", sw520Value);
    Firebase.RTDB.setFloat(&fbdo, "/Earthquake/Magnitude", magnitude);
    Firebase.RTDB.setString(&fbdo, "/Earthquake/Level", level);

    // Update status
    if (level == "High" || level == "Severe") {
      Firebase.RTDB.setString(&fbdo, "/Earthquake/Status", "WARNING: Earthquake Detected!");
    } else {
      Firebase.RTDB.setString(&fbdo, "/Earthquake/Status", "Normal");
    }
  }

  delay(500);
}
