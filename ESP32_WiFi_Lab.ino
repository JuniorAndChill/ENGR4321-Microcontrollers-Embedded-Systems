#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.h"

// --- STUDENT CONFIGURATION ---
const char* ssid = WIFI_SSID;  // Load these from header
const char* password = WIFI_PASS;  // Load these from header
//const char* ssid = "SpectrumSetup-XX"; // Your Router Name
//const char* password = "router_password"; // Your Router Password
const char* serverIP = "192.168.1.XXX"; // INSTRUCTOR'S PC IP ADDRESS
const char* studentName = "Table_X_ESP32Dev"; // Unique Name

// --- SECURITY KEYS (MUST MATCH SERVER) ---
const char* classToken = "ENGR4321_Spring2026_Secure";
const char* userAgent = "ESP32-Class-Device";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Construct the URL
    String serverPath = "http://" + String(serverIP) + ":8080/connect";
    
    http.begin(serverPath);
    
    // Add Security Headers
    http.addHeader("Content-Type", "application/json");
    http.addHeader("User-Agent", userAgent);
    http.addHeader("X-Auth-Token", classToken);

    // Create JSON Payload
    // In a real app, 'msg' could be sensor data
    String mac = WiFi.macAddress();
    String msg = "System Ready. Uptime: " + String(millis()/1000) + "s";
    
    String jsonPayload = "{\"id\":\"" + String(studentName) + "\", \"mac\":\"" + mac + "\", \"msg\":\"" + msg + "\"}";

    // Send POST Request
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Server Response: " + response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }

  // Send data every 5 seconds
  delay(5000);
}
