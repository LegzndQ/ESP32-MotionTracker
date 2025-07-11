#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include <WiFi.h>
#include <WiFiUdp.h>

class WiFiHandler {
public:
    void init(const char* ssid, const char* password, const IPAddress& remoteIP, int udpPort);
    
    // 发送加速度、角速度和四元数数据
    void sendData(float accelX, float accelY, float accelZ, 
                  float gyroX, float gyroY, float gyroZ,
                  float q_w, float q_x, float q_y, float q_z);
    // 发送加速度和角速度数据
    void sendData(float accelX, float accelY, float accelZ,
                  float gtroX, float gyroY, float gyroZ);
    // 部署时，可发送全部数据，显示姿态
    void sendData(float accelX, float accelY, float accelZ, 
                  float gyroX, float gyroY, float gyroZ,
                  float q_w, float q_x, float q_y, float q_z,
                  float motorASpeed, float motorBSpeed,
                  int pwmA, int pwmB);
    // 训练时，只需数据，无需四元数
    void sendData(float accelX, float accelY, float accelZ, 
                  float gyroX, float gyroY, float gyroZ,
                  float motorASpeed, float motorBSpeed,
                  int pwmA, int pwmB);
    

private:
    WiFiUDP udp;
    IPAddress remoteIP;
    int udpPort;
};

#endif