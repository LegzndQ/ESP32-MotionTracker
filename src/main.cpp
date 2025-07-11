/**
 * V3.0 增加红外遥控、电机控制模块与霍尔传感器读取，与Wi-Fi、MPU6050模块集成
 *      需注意HALL传感器和MPU6050模块的采样率匹配
 * V2.0 代码重构版本
 * V1.1 加速度和陀螺仪经过dmp处理版本，可与原始版本进行对比
 * 采样率为200Hz，匹配dmp处理的数据
 * 上电就一直输出数据
 * 可Wifi通信连接Python上位机
 */

#include "config.h"
#include "MPU6050Handler.h"
#include "WiFiHandler.h"
#include "InfraredReceiver.h"
#include "MotorControl.h"
#include "Hall.h"

const char* ssid = WIFI_SSID; // 在config.h中定义
const char* password = WIFI_PASSWORD;
const IPAddress remoteIP(192, 168, 1, 101); // 上位机的 IP 地址
const int udpPort = 12345; // 上位机的 UDP 端口

// 全局变量
bool collectingData = false;

// 全局对象
MotorControl motorControl;
IRControl irControl(IR_RECV_PIN, &motorControl, &collectingData);
MPU6050Handler mpuHandler;
WiFiHandler wifiHandler;
Hall hall;

void setup() {
    Serial.begin(115200); // 初始化串口通信

    motorControl.init();
    irControl.init();
    mpuHandler.init();
    hall.init();
    wifiHandler.init(ssid, password, remoteIP, udpPort);
}

void loop() {
    
    // 四元数
    float qW = 1.0f, qX = 0.0f, qY = 0.0f, qZ = 0.0f;
    HallData hallData = hall.readData(); // 读取霍尔传感器数据
    MPUData mpuData = mpuHandler.readData();
    
    if (mpuData.valid) { // 检查数据是否有效
        
        // 如果正在采集数据，发送数据到上位机
        if (collectingData) {
            wifiHandler.sendData(mpuData.accelX, mpuData.accelY, mpuData.accelZ, 
                mpuData.gyroX, mpuData.gyroY, mpuData.gyroZ, 
                mpuData.qW, mpuData.qX, mpuData.qY, mpuData.qZ,
                hallData.motorASpeed, hallData.motorBSpeed,
                motorControl.getMotorAPWM(), motorControl.getMotorBPWM()
            );
        }
 
    } else {
        Serial.println("FIFO 数据不足！ ");
    }

    irControl.processCommand(); // 处理红外遥控命令
       
    delay(10); // 每 10ms 发送一次数据（对应 100Hz）
    
}