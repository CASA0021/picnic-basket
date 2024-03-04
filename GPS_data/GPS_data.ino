#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TinyGPS++.h>

const char* ssid = "P30";
const char* password = "55559999";

TinyGPSPlus gps;
HardwareSerial SerialGPS(1); // Assuming GPS Module is connected to RX=22, TX=23

void setup() {
  Serial.begin(115200);
  SerialGPS.begin(9600, SERIAL_8N1, 22, 23); // Connect the GPS module to Serial1

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
}

void loop() {
  while (SerialGPS.available() > 0) {
    if (gps.encode(SerialGPS.read())) {
      if (gps.location.isValid() && gps.time.isValid()) {
        // GPS data is valid, proceed to get the weather data
        fetchAndPrintWeatherData(gps.location.lat(), gps.location.lng());
        delay(900000); // Delay for 15 minutes before the next data fetch
      }
    }
  }
}

void fetchAndPrintWeatherData(double lat, double lon) {
  String weatherApiUrl = String("https://api.open-meteo.com/v1/forecast") +
                         "?latitude=" + String(lat, 6) +
                         "&longitude=" + String(lon, 6) +
                         "&hourly=temperature_2m,relative_humidity_2m,rain,wind_speed_10m" +
                         "&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset" +
                         "&timezone=GMT&forecast_days=1";

  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(weatherApiUrl);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      DynamicJsonDocument doc(8192);
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        Serial.println("Weather Data:");

        // Hourly weather data
        Serial.println("Hourly Weather Data:");
        for (int i = 0; i < doc["hourly"]["time"].size(); i++) {
          Serial.print("Time: ");
          Serial.print(doc["hourly"]["time"][i].as<String>());
          Serial.print(", Temperature: ");
          Serial.print(doc["hourly"]["temperature_2m"][i].as<float>());
          Serial.println(" °C");
        }

        // Daily weather data
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
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
      }
    } else {
      Serial.print("HTTP request failed with code: ");
      Serial.println(httpCode);
    }

    http.end();
  } else {
    Serial.println("WiFi not connected");
  }
}
