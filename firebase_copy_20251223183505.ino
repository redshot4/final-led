#include <WiFi.h>
#include <Firebase_ESP_Client.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "Redshot"
#define WIFI_PASSWORD "1234567890"

/* 2. Define the API Key */
#define API_KEY "AIzaSyDtUtPAaJhTa04HZ6LMcJFxuW5EQ46oeBk"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://final-led-cb8f7-default-rtdb.asia-southeast1.firebasedatabase.app/"

/* 4. Define the user Email and password */
#define USER_EMAIL "karthi4444key@gmail.com"
#define USER_PASSWORD "Karthi2005@308"
        
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

const int led1 = 4;  // Example pin
const int led2 = 5;  // Another GPIO pin
const int led3 = 18; // Another GPIO pin
const int led4 = 19;
const int led5 = 21;

void setup() {
    Serial.begin(115200);
    pinMode(led1, OUTPUT);
    pinMode(led2, OUTPUT);
    pinMode(led3, OUTPUT);
    pinMode(led4, OUTPUT);
    pinMode(led5, OUTPUT);
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);
    digitalWrite(led5, LOW);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());

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
    if (Firebase.ready() && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)) {
        sendDataPrevMillis = millis();
        int state1, state2, state3, state4, state5;

        if (Firebase.RTDB.getInt(&fbdo, "/led/led1", &state1))
            digitalWrite(led1, state1);
        else
            Serial.println("LED1 Error: " + fbdo.errorReason());

        if (Firebase.RTDB.getInt(&fbdo, "/led/led2", &state2))
            digitalWrite(led2, state2);
        else
            Serial.println("LED2 Error: " + fbdo.errorReason());

        if (Firebase.RTDB.getInt(&fbdo, "/led/led3", &state3))
            digitalWrite(led3, state3);
        else
            Serial.println("LED3 Error: " + fbdo.errorReason());

        if (Firebase.RTDB.getInt(&fbdo, "/led/led4", &state4))
            digitalWrite(led4, state4);
        else
            Serial.println("LED4 Error: " + fbdo.errorReason());

        if (Firebase.RTDB.getInt(&fbdo, "/led/led5", &state5))
            digitalWrite(led5, state5);
        else
            Serial.println("LED5 Error: " + fbdo.errorReason());
        
    }
}
