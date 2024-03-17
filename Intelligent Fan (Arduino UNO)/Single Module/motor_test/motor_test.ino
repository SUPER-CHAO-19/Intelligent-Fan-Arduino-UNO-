// 定义电机控制引脚
const int motorPin1 = 10; // 这里假设你将电机连接到了Arduino的9号引脚
const int motorPin2 = 11;

void setup() {
  // 初始化串口通信
  Serial.begin(9600);
  // 设置电机控制引脚为输出模式
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
}

void loop() {

  analogWrite(motorPin1, 50); //
  analogWrite(motorPin2, 0); //
  Serial.println("电机100");
  delay(5000); // 电机转动2秒钟

  // 控制电机正转
  analogWrite(motorPin1, 100); //
  analogWrite(motorPin2, 0); //
  Serial.println("电机100");
  delay(5000); // 电机转动2秒钟
  
  // 控制电机停止
  analogWrite(motorPin1, 150); // 
  analogWrite(motorPin2, 0); //
  Serial.println("电机150");
  delay(5000); // 停止1秒钟

  analogWrite(motorPin1, 2000); // 
  analogWrite(motorPin2, 0); //
  Serial.println("电机200");
  delay(5000); // 停止1秒钟
  
}
