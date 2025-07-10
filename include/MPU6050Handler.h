#ifndef MPU6050HANDLER_H
#define MPU6050HANDLER_H

#include <MPU6050_6Axis_MotionApps20.h>

class MPU6050Handler {
public:
    void initialize();
    bool readData(float &accelX, float &accelY, float &accelZ,
        float &gyroX, float &gyroY, float &gyroZ,
        float &qW, float &qX, float &qY, float &qZ);

private:
    MPU6050 mpu;
};

#endif