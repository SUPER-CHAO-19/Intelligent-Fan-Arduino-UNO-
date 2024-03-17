#include <U8g2lib.h>
#include <OneWire.h>
#include <DallasTemperature.h>

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /*clock*/4, /*data*/3, /*cs*/2 , /*reset*/5);

#define ONE_WIRE_BUS 6                // DS18B20
#define ds18b20TimeInterval 1000
#define pin A5                        // 2Y0A21

// 89号引脚控制一个电机的两个引脚
const int motorPin1 = 10; // 第一个引脚连接到 8 号引脚
const int motorPin2 = 11; // 第二个引脚连接到 9 号引脚

unsigned long lastPersonDetectedTime = 0; // 上次检测到人的时间

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);  
DeviceAddress insideThermometer;

unsigned long ds18b20Times = 0;
float ds18b20Temp = 0;

void setup() {
  u8g2.begin();
  Serial.begin(9600);
  sensors.begin();
  if (!sensors.getAddress(insideThermometer, 0)) {
    Serial.println("设备未上线！");
  }
  Serial.println("设备上线！");
  pinMode(pin, INPUT);
  //电机
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
}

void loop() {
  printData();
}

void printData() {
  if (millis() - ds18b20Times >= ds18b20TimeInterval) {
    ds18b20Times = millis();
    sensors.requestTemperatures();
    ds18b20Temp = sensors.getTempC(insideThermometer);
    
    uint16_t value = analogRead(pin);
    uint16_t range = get_gp2d12(value);

    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_6x10_tr); // 使用较小的字体
      // 第一行显示温度
      u8g2.drawStr(0, 12, "Temp:");
      u8g2.setCursor(40, 12);
      u8g2.print(ds18b20Temp);
      u8g2.print(" C");

      // 第二行显示距离
      u8g2.drawStr(0, 24, "Distance:");
      u8g2.setCursor(60, 24);
      u8g2.print(range);
      u8g2.print(" mm");

      // 如果检测到人，则更新上次检测到人的时间
      if (range<1000){
        lastPersonDetectedTime = millis();
        if (ds18b20Temp<26){
          analogWrite(motorPin1, 0); //
          analogWrite(motorPin2, 0); //
        }
        else if (ds18b20Temp>=26 && ds18b20Temp<27){
          analogWrite(motorPin1, 50); //
          analogWrite(motorPin2, 0); //
        }
        else if (ds18b20Temp>=27 && ds18b20Temp<28){
          analogWrite(motorPin1, 75); //
          analogWrite(motorPin2, 0); //
        }
        else if (ds18b20Temp>=28 && ds18b20Temp<29){
          analogWrite(motorPin1, 100); //
          analogWrite(motorPin2, 0); //
        }
        else if(ds18b20Temp>=29 && ds18b20Temp<30){
          analogWrite(motorPin1, 150); //
          analogWrite(motorPin2, 0); //
        }
        else if(ds18b20Temp>=30){
          analogWrite(motorPin1, 200); //
          analogWrite(motorPin2, 0); //
        }
      }
      else{
        if (millis() - lastPersonDetectedTime >= 10000) {
          analogWrite(motorPin1, 0); //
          analogWrite(motorPin2, 0); //
        }
      }
    } while (u8g2.nextPage());
  }
}

// 计算距离
uint16_t get_gp2d12(uint16_t value) {
  if (value < 30)
    value = 30;
  return ((67870.0 / (value - 3.0)) - 40.0);
}
