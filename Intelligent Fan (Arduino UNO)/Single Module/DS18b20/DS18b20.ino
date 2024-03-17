/****************************************ds18b20 part****************************************/
#include <OneWire.h>                                          //调用库文件
#include <DallasTemperature.h>                                //调用库文件
#define ONE_WIRE_BUS 2                                        //设置对应的引脚
#define ds18b20TimeInterval 1000
 
OneWire oneWire(ONE_WIRE_BUS);                                //实例化对象
DallasTemperature sensors(&oneWire);                          //实例化对象
DeviceAddress insideThermometer;                              //实例化对象
 
unsigned long ds18b20Times = 0;
float ds18b20Temp = 0;
/****************************************set up and loop part*********************************/
void setup(void)
{
  Serial.begin(9600);                                         //设置串口波特率为9600
  sensors.begin();                                            //传感器波特率初始化
  if (!sensors.getAddress(insideThermometer, 0)) {            //检测设备是否上线
    Serial.println("设备未上线！");                             //串口打印对应的值
  }
  Serial.println("设备上线！");                                 //串口打印对应的值
}
void loop()
{
  printTemperature(insideThermometer);                         //获取温度数据
}
/****************************************ds18b20 part****************************************/
/*获取温度数据*/
void printTemperature(DeviceAddress deviceAddress)
{
  if (millis() - ds18b20Times >= ds18b20TimeInterval) {        //一定时间执行一次
    ds18b20Times = millis();
    sensors.requestTemperatures();                             //请求温度的数据
    ds18b20Temp = sensors.getTempC(deviceAddress);
 
    Serial.print("温度: ");                                    //串口打印对应的值
    Serial.print(ds18b20Temp);
    Serial.println(" (C).");
  }
}