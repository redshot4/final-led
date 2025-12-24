#include <WiFi.h>
#include <Firebase_ESP_Client.h>

/* 1. WIFI CREDENTIALS */
#define WIFI_SSID "Redshot"
#define WIFI_PASSWORD "1234567890"

/* 2. FIREBASE API & URL */
#define API_KEY "AIzaSyDtUtPAaJhTa04HZ6LMcJFxuW5EQ46oeBk"
#define DATABASE_URL "https://final-led-cb8f7-default-rtdb.asia-southeast1.firebasedatabase.app/"

/* 3. FIREBASE USER AUTH */
#define USER_EMAIL "karthi4444key@gmail.com"
#define USER_PASSWORD "Karthi2005@308"

// Firebase Objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Timers and Logic Variables
unsigned long sendDataPrevMillis = 0;
const long interval = 2000;      // Send data every 2 seconds
bool isNightMode = false;        // "Flag" to remember if we are in Dark Mode
const int darknessThreshold = 2500; // Value > 2500 is considered Dark

// --- PIN DEFINITIONS ---
// Module A (Relays 1 & 2)
const int relay1 = 26; 
const int relay2 = 27; // Controlled Automatically
// Module B (Relays 3 & 4)
const int relay3 = 25;
const int relay4 = 33; // Controlled Automatically
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

  // Set Relays to OFF (Active LOW: HIGH = OFF)
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.println("Connected!");

  // Firebase Configuration
  config.api_key = API_KEY;
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
    
    // --- 1. READ SENSOR ---
    int ldrValue = analogRead(ldrPin);

    // --- 2. AUTOMATION LOGIC (Runs every 2 seconds) ---
    if (millis() - sendDataPrevMillis > interval) {
      sendDataPrevMillis = millis();
      
      // A. Send LDR Value to Firebase (for the website display)
      Serial.print("LDR Value: ");
      Serial.println(ldrValue);
      Firebase.RTDB.setInt(&fbdo, "/sensor/ldr", ldrValue);

      // B. Check for Day/Night Transition
      
      // CASE: It is DARK (> 2500)
      if (ldrValue > darknessThreshold) {
        // Only run this ONCE when it *becomes* dark
        if (!isNightMode) {
          Serial.println("Darkness Detected -> Switching ON Relays 2 & 4");
          
          // Update Firebase (The Website will see ON, and the physical relay follows below)
          Firebase.RTDB.setInt(&fbdo, "/home/relay2", 1);
          Firebase.RTDB.setInt(&fbdo, "/home/relay4", 1);
          
          isNightMode = true; // Set flag so we don't spam the database
        }
      } 
      // CASE: It is BRIGHT (< 2500)
      else {
        // Only run this ONCE when it *becomes* bright
        if (isNightMode) {
          Serial.println("Light Detected -> Switching OFF Relays 2 & 4");
          
          // Update Firebase (The Website will see OFF)
          Firebase.RTDB.setInt(&fbdo, "/home/relay2", 0);
          Firebase.RTDB.setInt(&fbdo, "/home/relay4", 0);
          
          isNightMode = false; // Reset flag
        }
      }
    }

    // --- 3. PHYSICAL CONTROL (Executes Firebase State) ---
    // This part ensures the Relays always match what is in the Database.
    // Because the logic above updates the Database, this part handles the actual clicking.
    
    int rState;

    // Relay 1 (Manual)
    if (Firebase.RTDB.getInt(&fbdo, "/home/relay1", &rState)) 
      digitalWrite(relay1, rState == 1 ? LOW : HIGH);

    // Relay 2 (Auto + Manual)
    if (Firebase.RTDB.getInt(&fbdo, "/home/relay2", &rState)) 
      digitalWrite(relay2, rState == 1 ? LOW : HIGH);

    // Relay 3 (Manual)
    if (Firebase.RTDB.getInt(&fbdo, "/home/relay3", &rState)) 
      digitalWrite(relay3, rState == 1 ? LOW : HIGH);

    // Relay 4 (Auto + Manual)
    if (Firebase.RTDB.getInt(&fbdo, "/home/relay4", &rState)) 
      digitalWrite(relay4, rState == 1 ? LOW : HIGH);
  }
}
