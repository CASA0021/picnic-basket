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


#define DHTPIN 23     
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);


#define RXPin 33
#define TXPin 32
HardwareSerial gpsSerial(1);

TinyGPSPlus gps;
int h=0; 
int h1=0; 
int t=0;
 int t1=0;
int value = 123;
int uvLevel =0;
int longtitude=0;
int latitude=0;
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
  UWORD line3_y = width / 2;
/* Entry point ----------------------------------------------------------------*/
void setup()
{
  Serial.begin(115200);
    //dht.begin();


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


#if 0   // Drawing on the image
  //1.Select Image
  printf("SelectImage:BlackImage\r\n");
  Paint_SelectImage(BlackImage);
  Paint_Clear(WHITE);

  // 2.Drawing on the image
  printf("Drawing:BlackImage\r\n");
  Paint_DrawPoint(10, 80, BLACK, DOT_PIXEL_1X1, DOT_STYLE_DFT);
  Paint_DrawPoint(10, 90, BLACK, DOT_PIXEL_2X2, DOT_STYLE_DFT);
  Paint_DrawPoint(10, 100, BLACK, DOT_PIXEL_3X3, DOT_STYLE_DFT);
  Paint_DrawLine(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  Paint_DrawLine(70, 70, 20, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
  Paint_DrawRectangle(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  Paint_DrawCircle(45, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
  Paint_DrawCircle(105, 95, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
  Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
  Paint_DrawString_EN(10, 0, "waveshare", &Font16, BLACK, WHITE);
  Paint_DrawString_EN(10, 20, "hello world", &Font12, WHITE, BLACK);
  Paint_DrawNum(10, 33, 123456789, &Font12, BLACK, WHITE);
  Paint_DrawNum(10, 50, 987654321, &Font16, WHITE, BLACK);
  Paint_DrawString_CN(130, 0, " 你好", &Font12CN, BLACK, WHITE);
  Paint_DrawString_CN(130, 20, "微雪电子", &Font24CN, WHITE, BLACK);
  printf("EPD_Display\r\n");
  
  EPD_7IN5_V2_Display(BlackImage);
  DEV_Delay_ms(500);
#endif

#if 0 
  printf("Clear...\r\n");
  EPD_7IN5_V2_Clear();

  printf("Goto Sleep...\r\n");
  EPD_7IN5_V2_Sleep();
  free(BlackImage);
  BlackImage = NULL;
#endif
  Paint_Clear(WHITE); 
  Paint_DrawLine(line3_y, 0, line3_y, height, BLACK, DOT_PIXEL_5X5, LINE_STYLE_DOTTED);
  EPD_7IN5_V2_Display(BlackImage);
  delay(500);

}

/* The main loop -------------------------------------------------------------*/
void loop()
{ 
Paint_Clear(WHITE); 


     h = dht.readHumidity();
     if(h<30&&h>5)
     {h1=h;}

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
  
  
  sprintf(str, "%d", h);
  sprintf(str1, "%d", t);
  Paint_DrawString_EN(line3_y + 20, 20, "Advice:", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(20, 30, "Humidity:", &Font24, WHITE, BLACK);
  Paint_DrawNum(20, 60, h1, &Font24, WHITE, BLACK);
  Paint_DrawString_EN(60, 60, "%", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(20, 90, "Temperature:", &Font24, WHITE, BLACK);
  Paint_DrawNum(20, 120, t1, &Font24, WHITE, BLACK);
  Paint_DrawString_EN(60, 120, "C", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(20, 150, "UV level:", &Font24, WHITE, BLACK);
  Paint_DrawNum(20, 180, uvLevel, &Font24, WHITE, BLACK);
  Paint_DrawString_EN(20, 210, "Latitude:", &Font24, WHITE, BLACK);
  Paint_DrawNum(20, 240, latitude, &Font24, WHITE, BLACK);
  Paint_DrawString_EN(20, 270, "Longtitude:", &Font24, WHITE, BLACK);
  Paint_DrawNum(20, 300, longtitude, &Font24, WHITE, BLACK);
  Paint_DrawString_EN(20, 330, "Time:", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(50, 360, ":", &Font24, WHITE, BLACK);
  Paint_DrawNum(20, 360, gps.time.hour(), &Font24, WHITE, BLACK);
  Paint_DrawNum(70, 360, gps.time.minute(), &Font24, WHITE, BLACK);
  EPD_7IN5_V2_Display(BlackImage);

  delay(5000);

}
