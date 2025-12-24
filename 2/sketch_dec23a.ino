#include <WiFi.h>
#include <Firebase_ESP_Client.h>

/* 1. INSERT YOUR CREDENTIALS HERE */
#define WIFI_SSID "Redshot"
#define WIFI_PASSWORD "1234567890"  // Replace if different

#define API_KEY "AIzaSyDtUtPAaJhTa04HZ6LMcJFxuW5EQ46oeBk"
#define DATABASE_URL "https://final-led-cb8f7-default-rtdb.asia-southeast1.firebasedatabase.app/"

#define USER_EMAIL "karthi4444key@gmail.com"
#define USER_PASSWORD "Karthi2005@308"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
const long interval = 2000; 

// --- PIN DEFINITIONS ---
// Module A (Relays 1 & 2)
const int relay1 = 26; 
const int relay2 = 27;
// Module B (Relays 3 & 4)
const int relay3 = 25;
const int relay4 = 33;

// Sensor
const int ldrPin = 34; 

void setup() {
  Serial.begin(115200);

  // Initialize Pins
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  pinMode(ldrPin, INPUT);

  // Set Relays to OFF (Assuming Active LOW, so HIGH = OFF)
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.println("Connected!");

  // --- FIX IS HERE ---
  config.api_key = API_KEY;
  
  // Assign credentials to the 'auth' object directly
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  
  config.database_url = DATABASE_URL;

  Firebase.reconnectNetwork(true);
  fbdo.setBSSLBufferSize(4096, 1024);
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);
  Firebase.setDoubleDigits(5);
  config.timeout.serverResponse = 10 * 1000;
}

void loop() {
  if (Firebase.ready()) {
    
    // --- READ ALL 4 RELAYS ---
    int rState;

    // Relay 1
    if (Firebase.RTDB.getInt(&fbdo, "/home/relay1", &rState)) 
      digitalWrite(relay1, rState == 1 ? LOW : HIGH);

    // Relay 2
    if (Firebase.RTDB.getInt(&fbdo, "/home/relay2", &rState)) 
      digitalWrite(relay2, rState == 1 ? LOW : HIGH);

    // Relay 3
    if (Firebase.RTDB.getInt(&fbdo, "/home/relay3", &rState)) 
      digitalWrite(relay3, rState == 1 ? LOW : HIGH);

    // Relay 4
    if (Firebase.RTDB.getInt(&fbdo, "/home/relay4", &rState)) 
      digitalWrite(relay4, rState == 1 ? LOW : HIGH);

    // --- SEND LDR DATA ---
    if (millis() - sendDataPrevMillis > interval) {
      sendDataPrevMillis = millis();
      int ldrValue = analogRead(ldrPin);
      
      // Print to Serial Monitor for debugging
      Serial.print("LDR Value: ");
      Serial.println(ldrValue);
      
      if (Firebase.RTDB.setInt(&fbdo, "/sensor/ldr", ldrValue)) {
        // Success
      } else {
        Serial.println("Error sending LDR: " + fbdo.errorReason());
      }
    }
  }
}