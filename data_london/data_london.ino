#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "P30"; 
const char* password = "55559999"; 

// Open-Meteo API URL
const char* weatherApiUrl = "https://api.open-meteo.com/v1/forecast?latitude=51.5085&longitude=-0.1257&hourly=temperature_2m,relative_humidity_2m,rain,wind_speed_10m&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset&timezone=GMT&forecast_days=1";

void setup() {
  Serial.begin(115200);
  // connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(weatherApiUrl);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      DynamicJsonDocument doc(8192);
      deserializeJson(doc, payload);

      Serial.println("Hourly Weather Data:");
      for (int i = 0; i < doc["hourly"]["time"].size(); i++) {
        Serial.print("Time: ");
        Serial.print(doc["hourly"]["time"][i].as<String>());
        Serial.print(", Temperature: ");
        Serial.print(doc["hourly"]["temperature_2m"][i].as<float>());
        Serial.println(" °C");
      }

      Serial.println("\nDaily Weather Data:");
      Serial.print("Max Temperature: ");
      Serial.print(doc["daily"]["temperature_2m_max"][0].as<float>());
      Serial.print(" °C, Min Temperature: ");
      Serial.print(doc["daily"]["temperature_2m_min"][0].as<float>());
      Serial.println(" °C");
      Serial.print("Sunrise: ");
      Serial.print(doc["daily"]["sunrise"][0].as<String>());
      Serial.print(", Sunset: ");
      Serial.println(doc["daily"]["sunset"][0].as<String>());
    } else {
      Serial.println("Error on HTTP request");
    }

    http.end();
  }
  delay(60000);  // 每60秒更新一次
}