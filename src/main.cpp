/**
 * V2.0 代码重构版本
 * V1.1 加速度和陀螺仪经过dmp处理版本，可与原始版本进行对比
 * 采样率为200Hz，匹配dmp
 * 上电就一直输出数据
 * 可Wifi通信连接Python上位机
 */

 #include "MPU6050Handler.h"
 #include "WiFiHandler.h"

const char* ssid = "your_SSID"; // 替换为你的WiFi SSID
const char* password = "your_PASSWORD"; // 替换为你的WiFi密码
const IPAddress remoteIP(192, 168, 1, 101); // 上位机的 IP 地址
const int udpPort = 12345; // 上位机的 UDP 端口

MPU6050Handler mpuHandler;
WiFiHandler wifiHandler;

void setup() {
    Serial.begin(115200); // 初始化串口通信

    mpuHandler.initialize();
    wifiHandler.initialize(ssid, password, remoteIP, udpPort);
    
}

void loop() {
    float accelX, accelY, accelZ, 
          gyroX, gyroY, gyroZ;
    // 四元数
    float qW = 1.0f, qX = 0.0f, qY = 0.0f, qZ = 0.0f;
    
    if (mpuHandler.readData(accelX, accelY, accelZ, 
                            gyroX, gyroY, gyroZ, 
                            qW, qX, qY, qZ)) {
        // 发送数据到上位机
        wifiHandler.sendData(accelX, accelY, accelZ, 
            gyroX, gyroY, gyroZ, 
            qW, qX, qY, qZ);

    } else {
        Serial.println("FIFO 数据不足！ ");
    }
       
    delay(10); // 每 10ms 发送一次数据（对应 100Hz）
    
}