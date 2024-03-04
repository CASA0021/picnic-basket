#include <TinyGPS++.h>
#include <HardwareSerial.h>

const int RXPin = 23; // GPS模块TX连接到ESP32的RXPin
const int TXPin = 22; // GPS模块RX连接到ESP32的TXPin
const int GPSBaud = 9600; // GPS模块的波特率

// 初始化GPS模块的串行通信
HardwareSerial SerialGPS(1);

// 创建一个TinyGPS++对象
TinyGPSPlus gps;

void setup() {
  Serial.begin(115200); // 初始化与PC的串行通信
  SerialGPS.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin); // 初始化与GPS模块的串行通信

  Serial.println("GPS Module Test");
}

void loop() {
  // 持续读取从GPS模块收到的数据
  while (SerialGPS.available() > 0) {
    if (gps.encode(SerialGPS.read())) { // 如果读取到新的有效数据
      displayGPSInfo(); // 显示GPS信息
    }
  }
}

void displayGPSInfo() {
  // 如果定位有效
  if (gps.location.isValid()) {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6); // 显示纬度，保留6位小数
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6); // 显示经度，保留6位小数
  } else {
    Serial.println("Location: Not Available");
  }

  // 显示时间信息
  if (gps.time.isValid()) {
    Serial.print("Time: ");
    Serial.print(gps.time.hour());
    Serial.print(":");
    Serial.print(gps.time.minute());
    Serial.print(":");
    Serial.println(gps.time.second());
  } else {
    Serial.println("Time: Not Available");
  }

  Serial.println(); // 在信息之间添加空行
  delay(1000); // 每秒更新一次
}
