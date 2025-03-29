#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "  "; // your hotspot name
const char* password = " "; //Your hotspot password

// Replace with your ThingSpeak details
const char* readAPI = "http://api.thingspeak.com/channels/2848961/fields/1/last.txt";
const char* writeAPI = "http://api.thingspeak.com/update?api_key=  ";//add the api key

#define RELAY_PIN D1  // Pin for relay/servo

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // Keep door locked initially
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, readAPI);  // Read from ThingSpeak

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      String response = http.getString();
      Serial.println("ThingSpeak Response: " + response);

      if (response.toInt() == 1) {  // ✅ Unlock only when "1" is received
        Serial.println("🔓 Unlocking Door...");
        digitalWrite(RELAY_PIN, HIGH);  // Activate relay/servo
        delay(5000);  // Keep unlocked for 5 seconds
        Serial.println("🔒 Locking Door...");
        digitalWrite(RELAY_PIN, LOW);  // Lock again

        // ✅ Reset field1 to "0" in ThingSpeak to prevent re-unlocking
        HTTPClient httpWrite;
        httpWrite.begin(client, writeAPI);
        httpWrite.GET();
        httpWrite.end();
      }
    } else {
      Serial.println("Failed to get data from ThingSpeak!");
    }

    http.end();
  }
  
  delay(5000); // Check ThingSpeak every 5 seconds
}

