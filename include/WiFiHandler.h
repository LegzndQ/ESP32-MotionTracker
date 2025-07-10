#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include <WiFi.h>
#include <WiFiUdp.h>

class WiFiHandler {
public:
    void initialize(const char* ssid, const char* password, const IPAddress& remoteIP, int udpPort);
    void sendData(float accelX, float accelY, float accelZ, 
                  float gyroX, float gyroY, float gyroZ,
                  float q_w, float q_x, float q_y, float q_z);

private:
    WiFiUDP udp;
    IPAddress remoteIP;
    int udpPort;
};

#endif