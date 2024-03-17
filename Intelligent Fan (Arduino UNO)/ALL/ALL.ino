#include <IRremote.h>
// #include <U8g2lib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <U8glib.h>

#define irReceiverPin 7
#define motorPin1 10
#define motorPin2 11
#define ONE_WIRE_BUS 6
#define ds18b20TimeInterval 1000
#define pin A5
#define pushButton A3

IRrecv irrecv(irReceiverPin);
decode_results results;
//U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /*clock*/4, /*data*/3, /*cs*/2, /*reset*/5);
U8GLIB_ST7920_128X64_4X u8g(4, 3, 2);

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress insideThermometer;

unsigned long ds18b20Times = 0;
unsigned long lastPersonDetectedTime = 0;
float ds18b20Temp = 0;

unsigned long manu_flag = 0; // 存储红外信号值

bool automaticMode = true; // 初始为自动模式

int buttonState = 0;// 按键次数

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
  // u8g2.begin();
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }

  sensors.begin();
  sensors.getAddress(insideThermometer, 0);

  pinMode(pin, INPUT);
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(pushButton, INPUT);
}

void loop() {
  if (irrecv.decode(&results)) {
    if (results.value == 0xFFA25D) { // 如果按下红外遥控的 CH- 按钮
      automaticMode = true; // 切换到自动模式
    } else if (results.value == 0xFFE21D) { // 如果按下红外遥控的 CH+ 按钮
      automaticMode = false; // 切换到遥控模式
    }
    irrecv.resume(); // 准备接收下一个红外信号
  }

  if (automaticMode) {
    controlFanAutomatic();
  } else {
    controlFanManual();
  }
}

void controlFanAutomatic() {
  if (millis() - ds18b20Times >= ds18b20TimeInterval) {
    ds18b20Times = millis();
    sensors.requestTemperatures();
    ds18b20Temp = sensors.getTempC(insideThermometer);

    uint16_t value = analogRead(pin);
    uint16_t range = get_gp2d12(value);

    // u8g2.firstPage();
    u8g.firstPage(); 
    do {
      u8g.setFont(u8g_font_6x10); // 使用较小的字体
      u8g.drawStr(0, 12, "Auto");

      // 第一行显示温度
      u8g.drawStr(0, 24, "Temp:");
      u8g.setPrintPos(40, 24);
      u8g.print(ds18b20Temp);
      u8g.print(" C");

      // 第二行显示距离
      u8g.drawStr(0, 36, "Distance:");
      u8g.setPrintPos(60, 36);
      u8g.print(range);
      u8g.print(" mm");

      // 如果检测到人，则更新上次检测到人的时间
      if (range < 500) {
        lastPersonDetectedTime = millis();
        if (ds18b20Temp < 26) {
          analogWrite(motorPin1, 40);
          analogWrite(motorPin2, 0);
          u8g.drawStr(40, 12, "Mode0");
        } else if (ds18b20Temp >= 26 && ds18b20Temp < 27) {
          analogWrite(motorPin1, 50);
          analogWrite(motorPin2, 0);
          u8g.drawStr(40, 12, "Mode1");
        } else if (ds18b20Temp >= 27 && ds18b20Temp < 28) {
          analogWrite(motorPin1, 75);
          analogWrite(motorPin2, 0);
          u8g.drawStr(40, 12, "Mode2");
        } else if (ds18b20Temp >= 28 && ds18b20Temp < 29) {
          analogWrite(motorPin1, 100);
          analogWrite(motorPin2, 0);
          u8g.drawStr(40, 12, "Mode3");
        } else if (ds18b20Temp >= 29 && ds18b20Temp < 30) {
          analogWrite(motorPin1, 150);
          analogWrite(motorPin2, 0);
          u8g.drawStr(40, 12, "Mode4");
        } else if (ds18b20Temp >= 30) {
          analogWrite(motorPin1, 200);
          analogWrite(motorPin2, 0);
          u8g.drawStr(40, 12, "Mode5");
        }
      } else {
        // 如果30秒内没有检测到人，则关闭电机
        if (millis() - lastPersonDetectedTime >= 30000) {
          analogWrite(motorPin1, 0);
          analogWrite(motorPin2, 0);
        }
      }
    } while (u8g.nextPage());
  }
}

// 计算距离
uint16_t get_gp2d12(uint16_t value) {
  if (value < 30)
    value = 30;
  return ((67870.0 / (value - 3.0)) - 40.0);
}
 
void controlFanManual() {
  if (millis() - ds18b20Times >= ds18b20TimeInterval) {
    ds18b20Times = millis();
    sensors.requestTemperatures();
    ds18b20Temp = sensors.getTempC(insideThermometer);

    uint16_t value = analogRead(pin);
    uint16_t range = get_gp2d12(value);

    // u8g2.firstPage();
    u8g.firstPage(); 
    do {
      u8g.setFont(u8g_font_6x10); // 使用较小的字体
      u8g.drawStr(0, 12, "Manu");

      // 第一行显示温度
      u8g.drawStr(0, 24, "Temp:");
      u8g.setPrintPos(40, 24);
      u8g.print(ds18b20Temp);
      u8g.print(" C");

      // 第二行显示距离
      u8g.drawStr(0, 36, "Distance:");
      u8g.setPrintPos(60, 36);
      u8g.print(range);
      u8g.print(" mm");

      if (digitalRead(pushButton) == 0){
        delay(20);
        while(digitalRead(pushButton) == 0);
        delay(20);
        buttonState++;
        if (buttonState == 6){
            buttonState = 0;
        }
      }

      if (buttonState == 0) { // 按键0
        analogWrite(motorPin1, 0); // 停止电机
        analogWrite(motorPin2, 0); // 
        u8g.drawStr(40, 12, "Mode0");
        buttonState = 0;
      }
      else if (buttonState == 1) { // 按键1
        analogWrite(motorPin1, 50); //
        analogWrite(motorPin2, 0); //
        u8g.drawStr(40, 12, "Mode1");
        buttonState = 1;
      }
      else if (buttonState == 2) { // 按键2
        analogWrite(motorPin1, 75); // 
        analogWrite(motorPin2, 0); //
        u8g.drawStr(40, 12, "Mode2"); 
        buttonState = 2;
      }
      else if (buttonState == 3) { // 按键3
        analogWrite(motorPin1, 100); //
        analogWrite(motorPin2, 0); //
        u8g.drawStr(40, 12, "Mode3"); 
        buttonState = 3;
      }
      else if (buttonState == 4) { // 按键4
        analogWrite(motorPin1, 150); //
        analogWrite(motorPin2, 0); //
        u8g.drawStr(40, 12, "Mode4"); 
        buttonState = 4;
      }
      else if (buttonState == 5) { // 按键5
        analogWrite(motorPin1, 200); //
        analogWrite(motorPin2, 0); //
        u8g.drawStr(40, 12, "Mode5"); 
        buttonState = 5;
      }

      if (irrecv.decode(&results)) {
        // 打印接收到的红外信号的值和位数
        Serial.print("irCode: ");
        Serial.print(results.value, HEX);
        Serial.print(", bits: ");
        Serial.println(results.bits);
        
        manu_flag = results.value;
        // 根据不同的红外信号值来控制电机转速
        if (results.value == 0xFF6897) { // 按键0
          // analogWrite(motorPin1, 0); // 停止电机
          // analogWrite(motorPin2, 0); // 
          // u8g.drawStr(40, 12, "Mode0");
          buttonState = 0;
        }
        else if (results.value == 0xFF30CF) { // 按键1
          // analogWrite(motorPin1, 50); //
          // analogWrite(motorPin2, 0); //
          // u8g.drawStr(40, 12, "Mode1");
          buttonState = 1;
        }
        else if (results.value == 0xFF18E7) { // 按键2
          // analogWrite(motorPin1, 75); // 
          // analogWrite(motorPin2, 0); //
          // u8g.drawStr(40, 12, "Mode2"); 
          buttonState = 2;
        }
        else if (results.value == 0xFF7A85) { // 按键3
          // analogWrite(motorPin1, 100); //
          // analogWrite(motorPin2, 0); //
          // u8g.drawStr(40, 12, "Mode3"); 
          buttonState = 3;
        }
        else if (results.value == 0xFF10EF) { // 按键3
          // analogWrite(motorPin1, 150); //
          // analogWrite(motorPin2, 0); //
          // u8g.drawStr(40, 12, "Mode4"); 
          buttonState = 4;
        }
        else if (results.value == 0xFF38C7) { // 按键3
          // analogWrite(motorPin1, 200); //
          // analogWrite(motorPin2, 0); //
          // u8g.drawStr(40, 12, "Mode5"); 
          buttonState = 5;
        }
        else {
        // 其他按键值不进行任何操作
        }
        delay(300);
        irrecv.resume(); // 准备接收下一个红外信号
      } 
      // if (manu_flag == 0xFF6897) { // 按键0
      //   analogWrite(motorPin1, 0); // 停止电机
      //   analogWrite(motorPin2, 0); // 
      //   u8g.drawStr(40, 12, "Mode0");
      //   buttonState = 0
      // }
      // else if (manu_flag == 0xFF30CF) { // 按键1
      //   analogWrite(motorPin1, 50); //
      //   analogWrite(motorPin2, 0); //
      //   u8g.drawStr(40, 12, "Mode1");
      //   buttonState = 1
      // }
      // else if (manu_flag == 0xFF18E7) { // 按键2
      //   analogWrite(motorPin1, 75); // 
      //   analogWrite(motorPin2, 0); //
      //   u8g.drawStr(40, 12, "Mode2"); 
      //   buttonState = 2
      // }
      // else if (manu_flag == 0xFF7A85) { // 按键3
      //   analogWrite(motorPin1, 100); //
      //   analogWrite(motorPin2, 0); //
      //   u8g.drawStr(40, 12, "Mode3"); 
      //   buttonState = 3
      // }
      // else if (manu_flag == 0xFF10EF) { // 按键4
      //   analogWrite(motorPin1, 150); //
      //   analogWrite(motorPin2, 0); //
      //   u8g.drawStr(40, 12, "Mode4"); 
      //   buttonState = 4
      // }
      // else if (manu_flag == 0xFF38C7) { // 按键5
      //   analogWrite(motorPin1, 200); //
      //   analogWrite(motorPin2, 0); //
      //   u8g.drawStr(40, 12, "Mode5"); 
      //   buttonState = 5
      // }
      // else {
      //   // 其他按键值不进行任何操作
      // }
    }while (u8g.nextPage());
  }
}