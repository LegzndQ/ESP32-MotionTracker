#ifndef MPU6050HANDLER_H
#define MPU6050HANDLER_H

#include <MPU6050_6Axis_MotionApps20.h>

struct MPUData {
    float accelX, accelY, accelZ;
    float gyroX, gyroY, gyroZ;
    float qW, qX, qY, qZ; // 四元数
    unsigned long timestamp;
    bool valid; // 数据是否有效

    MPUData() : accelX(0), accelY(0), accelZ(0), 
                gyroX(0), gyroY(0), gyroZ(0), 
                qW(1), qX(0), qY(0), qZ(0), 
                timestamp(0), valid(false) {}
};

class MPU6050Handler {
private:
    MPU6050 mpu;

public:
    MPU6050Handler();
    void init();
    MPUData readData();
};

#endif