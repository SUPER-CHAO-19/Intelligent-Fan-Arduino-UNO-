#include <IRremote.h>
// 7号引脚接收红外信号
const int irReceiverPin = 7;
// 89号引脚控制一个电机的两个引脚
const int motorPin1 = 10; // 第一个引脚连接到 8 号引脚
const int motorPin2 = 11; // 第二个引脚连接到 9 号引脚


IRrecv irrecv(irReceiverPin);
decode_results results;

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
}

void loop() {
  if (irrecv.decode(&results)) {
    // 打印接收到的红外信号的值和位数
    Serial.print("irCode: ");
    Serial.print(results.value, HEX);
    Serial.print(", bits: ");
    Serial.println(results.bits);
    
    // 根据不同的红外信号值来控制电机转速
    if (results.value == 0xFF6897) { // 按键0
      analogWrite(motorPin1, 0); // 停止电机
      analogWrite(motorPin2, 0); // 
    }
    else if (results.value == 0xFF30CF) { // 按键1
      analogWrite(motorPin1, 100); //
      analogWrite(motorPin2, 0); //
    }
    else if (results.value == 0xFF18E7) { // 按键2
      analogWrite(motorPin1, 150); // 
      analogWrite(motorPin2, 0); // 
    }
    else if (results.value == 0xFF7A85) { // 按键3
      analogWrite(motorPin1, 200); //
      analogWrite(motorPin2, 0); // 
    }
    else {
    // 其他按键值不进行任何操作
    }
    delay(300);
    irrecv.resume(); // 准备接收下一个红外信号
  }
}
