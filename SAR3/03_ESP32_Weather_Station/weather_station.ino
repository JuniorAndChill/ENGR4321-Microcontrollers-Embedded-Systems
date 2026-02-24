#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <Wire.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

// WiFi credentials - Wokwi uses "Wokwi-GUEST" with no password
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Your Domain name with URL path or IP address with path
String openWeatherMapApiKey = "API_KEY_HERE";

// Replace with your country code and city
// Use %20 for spaces (not just %)
String city = "New%20Braunfels";
String countryCode = "US";

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;
String jsonBuffer;

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// SDA -> GPIO21, SCL -> GPIO22
// lcd address is 0x27
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

// NTP Server time
const char* ntpServer = "pool.ntp.org";
long gmtOffset_sec = 0;
int daylightOffset_sec = 0;

void setup() {
  Serial.begin(115200);
  
  // Initialize LCD first to show status
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting WiFi");
  
  // WiFi connection
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected!");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(2000);
  
  Serial.println("Timer set to 10 seconds (timerDelay variable)");
  
  // Initial fetch
  lastTime = millis() - timerDelay;
}

void loop() {
  // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&units=metric" + "&APPID=" + openWeatherMapApiKey;
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      
      JSONVar myObject = JSON.parse(jsonBuffer);
      
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Parse Error!");
        lastTime = millis();
        return;
      }
      
      // Check if API returned an error
      if (myObject.hasOwnProperty("cod")) {
        int code = (int)myObject["cod"];
        if (code != 200) {
          Serial.print("API Error: ");
          Serial.println((const char*)myObject["message"]);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("API Error:");
          lcd.setCursor(0, 1);
          lcd.print(code);
          lastTime = millis();
          return;
        }
      }
      
      // Get time
      gmtOffset_sec = (long)myObject["timezone"];
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
      
      // Wait a moment for time sync
      delay(1000);
      
      String time = printLocalTime();
      
      // Get weather data
      String weatherMain = (const char*)myObject["weather"][0]["main"];
      double temp = (double)myObject["main"]["temp"];
      int humidity = (int)myObject["main"]["humidity"];
      
      // LCD Print
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(time);
      lcd.print(" ");
      
      // Truncate weather description if too long
      if (weatherMain.length() > 10) {
        weatherMain = weatherMain.substring(0, 10);
      }
      lcd.print(weatherMain);
      
      lcd.setCursor(0, 1);
      lcd.print("T:");
      lcd.print(temp, 1);  // 1 decimal place
      lcd.print((char)223);  // degree symbol
      lcd.print("C ");
      lcd.print("H:");
      lcd.print(humidity);
      lcd.print("%");
      
      // Also print to Serial for debugging
      Serial.print("Time: ");
      Serial.println(time);
      Serial.print("Weather: ");
      Serial.println(weatherMain);
      Serial.print("Temp: ");
      Serial.print(temp);
      Serial.println(" C");
      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.println("%");
    }
    else {
      Serial.println("WiFi Disconnected");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WiFi Lost!");
      
      // Try to reconnect
      WiFi.begin(ssid, password);
    }
    lastTime = millis();
  }
}

String printLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "--:--";
  }
  
  char timeHour[3];
  strftime(timeHour, 3, "%H", &timeinfo);
  
  char timeMinute[3];
  strftime(timeMinute, 3, "%M", &timeinfo);
  
  String time = String(timeHour) + ":" + String(timeMinute);
  Serial.println(time);
  return time;
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
  
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Set timeout for Wokwi simulation
  http.setTimeout(10000);
  
  // Send HTTP GET request
  int httpResponseCode = http.GET();
  
  String payload = "{}";
  
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  
  // Free resources
  http.end();
  
  return payload;
}
