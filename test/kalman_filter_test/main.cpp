#include <Arduino.h>
#include "config.h"
#include "motor_control.h"
#include "sensors.h"
#include "kalman_filter.h"
#include "ir_control.h"
#include "communication.h"

// 全局对象
MotorControl motorControl;
Sensors sensors;
KalmanFilterManager kalmanFilter;
IRControl irControl(IR_RECV_PIN, &motorControl, &collectingData);
Communication communication;

// 全局变量
bool collectingData = false;
unsigned long lastTime = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32-AI 启动中...");
    
    // 初始化各模块
    motorControl.init();
    sensors.init();
    kalmanFilter.init();
    irControl.init();
    
    // 初始化 WiFi 和 UDP 通信
    if (communication.initWiFi()) {
        communication.initUDP();
    } else {
        Serial.println("WiFi 连接失败，系统将继续运行但无法发送数据");
    }
    
    Serial.println("系统初始化完成");
}

void loop() {
    // 处理传感器数据
    if (sensors.dataReady()) {
        SensorData sensorData = sensors.readSensorData();
        
        // 计算时间间隔
        unsigned long currentTime = millis();
        float dt = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        // 应用卡尔曼滤波
        float filteredAccelX, filteredAccelY, filteredAccelZ;
        float filteredGyroX, filteredGyroY, filteredGyroZ;
        
        kalmanFilter.filterSensorData(
            sensorData.accelX, sensorData.accelY, sensorData.accelZ,
            sensorData.gyroX, sensorData.gyroY, sensorData.gyroZ,
            dt,
            filteredAccelX, filteredAccelY, filteredAccelZ,
            filteredGyroX, filteredGyroY, filteredGyroZ
        );
        
        // 打印滤波后的数据
        Serial.printf("滤波后数据: AccelX=%.2f, AccelY=%.2f, AccelZ=%.2f, GyroX=%.2f, GyroY=%.2f, GyroZ=%.2f\n",
                      filteredAccelX, filteredAccelY, filteredAccelZ, 
                      filteredGyroX, filteredGyroY, filteredGyroZ);
        
        // 如果正在采集数据且 WiFi 连接正常，发送数据
        if (collectingData && communication.isConnected()) {
            communication.sendSensorData(
                filteredAccelX, filteredAccelY, filteredAccelZ,
                filteredGyroX, filteredGyroY, filteredGyroZ,
                sensorData.motorASpeed, sensorData.motorBSpeed,
                motorControl.getMotorAPWM(), motorControl.getMotorBPWM()
            );
        }
    }
    
    // 处理红外遥控命令
    irControl.processCommand();
    
    // 主循环延迟
    delay(mainLoopDelay);
}