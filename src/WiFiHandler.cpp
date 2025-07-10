#include "WiFiHandler.h"

void WiFiHandler::initialize(const char* ssid, const char* password, const IPAddress& remoteIP, int udpPort) {
    this->udpPort = udpPort;
    this->remoteIP = remoteIP;
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi 连接成功");
    
    udp.begin(udpPort);
}

/**
 * @brief 发送数据到上位机
 * @param accelX 加速度 X 轴数据
 * @param accelY 加速度 Y 轴数据
 * @param accelZ 加速度 Z 轴数据
 * @param gyroX 陀螺仪 X 轴数据
 * @param gyroY 陀螺仪 Y 轴数据
 * @param gyroZ 陀螺仪 Z 轴数据
 * @param q_w 四元数 w 分量
 * @param q_x 四元数 x 分量
 * @param q_y 四元数 y 分量
 * @param q_z 四元数 z 分量
 */
void WiFiHandler::sendData(float accelX, float accelY, float accelZ, 
                      float gyroX, float gyroY, float gyroZ,
                      float q_w, float q_x, float q_y, float q_z) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%ld,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
                millis(), accelX, accelY, accelZ, gyroX, gyroY, gyroZ, q_w, q_x, q_y, q_z);
    udp.beginPacket(remoteIP, udpPort);
    udp.print(buffer);
    udp.endPacket();
}