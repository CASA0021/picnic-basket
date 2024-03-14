/* Includes ------------------------------------------------------------------*/
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "imagedata.h"
#include <stdlib.h>
#include <WiFi.h>
#include <ezTime.h>
#include "SPIFFS.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <TinyGPS++.h>
#include <string>


#define DHTPIN 23     
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define RXPin 33
#define TXPin 32
HardwareSerial gpsSerial(1);

TinyGPSPlus gps;
#define LED_PIN     22
#define LED_COUNT   8
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// UVsensor variable
int sensorValue;
long sum = 0;
int vout = 0;
int uv = 0;
unsigned long previousMillis = 0; 
const long interval = 500;        
bool ledState = false;
float h=0; 
float h1=0; 
float t=0;
float t1=0;
int i=0;
float j=1.234567;
float k=1.234567;
int value = 123;
int uvLevel =0;
float longtitude=0;
float latitude=0;
uint8_t hour=0;
uint8_t minute=0;
const char* ssid     = "P30";
const char* password = "55559999";

const char* weatherApiUrl = "https://api.open-meteo.com/v1/forecast?latitude=51.5085&longitude=-0.1257&hourly=temperature_2m,relative_humidity_2m,rain,wind_speed_10m&daily=temperature_2m_max,temperature_2m_min,sunrise,sunset&timezone=GMT&forecast_days=1";

  //Create a new image cache
  UBYTE *BlackImage;
  /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
  UWORD Imagesize = ((EPD_7IN5_V2_WIDTH % 8 == 0) ? (EPD_7IN5_V2_WIDTH / 8 ) : (EPD_7IN5_V2_WIDTH / 8 + 1)) * EPD_7IN5_V2_HEIGHT;

  PAINT_TIME sys_time;
  UWORD width = EPD_7IN5_V2_WIDTH;
  UWORD height = EPD_7IN5_V2_HEIGHT;

 
  UWORD line1_y = height / 3;
  UWORD line2_y = height* 2/3;
  UWORD line3_y = width / 3;
/* Entry point ----------------------------------------------------------------*/
void setup()
{
  Serial.begin(115200);
  dht.begin();
  strip.begin();                   
  strip.show();                    
  strip.setBrightness(50);         

  gpsSerial.begin(9600, SERIAL_8N1, RXPin, TXPin);
  Serial.println("GPS Demo开始");

  delay(100);
  printf("EPD_7IN5_V2_test Demo\r\n");
  DEV_Module_Init();

  printf("e-Paper Init and Clear...\r\n");
  EPD_7IN5_V2_Init();
  EPD_7IN5_V2_Clear();
  DEV_Delay_ms(500);

  if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
    printf("Failed to apply for black memory...\r\n");
    while (1);
  }
  printf("Paint_NewImage\r\n");
  Paint_NewImage(BlackImage, EPD_7IN5_V2_WIDTH, EPD_7IN5_V2_HEIGHT, 0, WHITE);
  Paint_Clear(WHITE); 
Paint_DrawImage(sun1, 0, 0, 800, 480);
  EPD_7IN5_V2_Display(BlackImage);
  delay(500);

}

/* The main loop -------------------------------------------------------------*/
void loop()
{ 

  sensorValue = 0;
  sum = 0;
  for(int i = 0; i < 1024; i++) {
    sensorValue = analogRead(A0);
    sum += sensorValue;
    delay(2);
  }

  vout = sum >> 10;                
  vout = vout * 4980.0 / 1024;     
  Serial.print("The Photocurrent value: ");
  Serial.print(vout);
  Serial.println("mV");

  uv = calculateUVIndex(vout);     
  Serial.print("UV Index = ");
  Serial.println(uv);

  setLEDsForUVIndex(uv);           
  delay(1000); 

Paint_Clear(WHITE); 


     h = dht.readHumidity();
     if(h<100&&h>5)
     {
      h1=h;
     }

     t = dht.readTemperature();
     if(t<30&&t>0)
     {
      t1=t;
     }
  Paint_DrawLine(line3_y, 0, line3_y, height, BLACK, DOT_PIXEL_5X5, LINE_STYLE_DOTTED);
    

   uvLevel = analogRead(18);


  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print("%  Temperature: ");
    Serial.print(t);
    Serial.println("C ");
  }


  Serial.print("UV Level: ");
  Serial.println(uvLevel);


  bool newData = false;
  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();
    if (gps.encode(c)) {
      newData = true;
    }
  }

  if (newData) {
    if (gps.location.isValid()) {
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);
    } else {
      Serial.println("Location: not available");
    }
  latitude=gps.location.lat();
  longtitude=gps.location.lng();
    if (gps.time.isValid()) {
      Serial.print("Time: ");
      Serial.print(gps.time.hour());
      Serial.print(":");
      Serial.print(gps.time.minute());
      Serial.print(":");
      Serial.println(gps.time.second());
    } else {
      Serial.println("Time: not available");
    }
  } else {
    Serial.println("GPS signal not available...");
  }
  delay(2000);
  value = value +1;  
  char str[20]; 
  char str1[20];   
  char str2[20];
  char str3[20];
  sprintf(str, "%f", longtitude);
  sprintf(str1, "%f", latitude);
  sprintf(str2, "%.1f", h1);
  sprintf(str3, "%.1f", t1);  
  if(i<4)
  {
    i=i+1;
  }
  if(i==4)
  {
    i=0;
  }
  switch(i) {
        case 1:
            Paint_DrawImage(sun, line3_y + 35, 0, 480, 480);
            break;
        case 2:
            Paint_DrawImage(green, line3_y + 35, 0, 480, 480);
            break;
        case 3:
            Paint_DrawImage(medicine, line3_y + 35, 0, 480, 480);
            break;
        case 4:
            Paint_DrawImage(mushroom, line3_y + 35, 0, 480, 480);
            break;
        default:
            Paint_DrawImage(uv2, line3_y + 35, 0, 480, 480);
    }


  Paint_DrawString_EN(20, 30, "Humidity:", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(20, 60, str2, &Font24, BLACK, WHITE);
  Paint_DrawString_EN(90, 60, "%", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(20, 90, "Temperature:", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(20, 120, str3, &Font24, BLACK, WHITE);
  Paint_DrawChar(88, 120, 0x27, &Font24, BLACK, WHITE);
  Paint_DrawString_EN(95, 120, "C", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(20, 150, "Time:", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(54, 180, ":", &Font24, WHITE, BLACK);
  Paint_DrawNum(20, 180, gps.time.hour(), &Font24, WHITE, BLACK);
  Paint_DrawNum(70, 180, gps.time.minute(), &Font24, WHITE, BLACK);
  Paint_DrawString_EN(20, 210, "Latitude:", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(20, 240, str1, &Font24, BLACK, WHITE);
  Paint_DrawString_EN(20, 270, "Longtitude:", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(20, 300, str, &Font24, BLACK, WHITE);
  Paint_DrawImage(casa, 24, 333, 102, 137);
  EPD_7IN5_V2_Display(BlackImage);
  delay(25000);

}


int calculateUVIndex(int vout) {
  if(vout < 50) return 0;
  else if(vout < 227) return 1;
  else if(vout < 318) return 2;
  else if(vout < 408) return 3;
  else if(vout < 503) return 4;
  else if(vout < 606) return 5;
  else if(vout < 696) return 6;
  else if(vout < 795) return 7;
  else if(vout < 881) return 8;
  else if(vout < 976) return 9;
  else if(vout < 1079) return 10;
  else return 11;
}

void setLEDsForUVIndex(int uvIndex) {
  unsigned long currentMillis = millis();

  if (uvIndex < 11) {
    
    strip.clear();
    int blueIntensity = map(uvIndex, 0, 10, 0, 255);
    int ledsToLight = 0;
    if (uvIndex >= 0 && uvIndex <= 2) {
      ledsToLight = 2;
    } else if (uvIndex >= 3 && uvIndex <= 5) {
      ledsToLight = 4;
    } else if (uvIndex >= 6 && uvIndex <= 7) {
      ledsToLight = 6;
    } else if (uvIndex >= 8 && uvIndex <= 10) {
      ledsToLight = 8;
    }

    
    for (int i = 0; i < ledsToLight; i++) {
      strip.setPixelColor(i, strip.Color(0, 0, blueIntensity));
    }
    
    
    strip.show();
  } else {
    
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis; 

      ledState = !ledState; 

      if (ledState) {
        for (int i = 0; i < LED_COUNT; i++) {
          
          strip.setPixelColor(i, strip.Color(255, 255, 0));
        }
      } else {
        
        strip.clear();
      }
      
      strip.show();
    }
  }
}