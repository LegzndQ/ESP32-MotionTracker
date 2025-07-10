#include "MPU6050Handler.h"
#include <Wire.h>

void MPU6050Handler::initialize() {
    Wire.begin(21, 20); // 初始化I2C总线，SDA、SCL引脚
    mpu.initialize(); // 初始化MPU6050

	uint8_t devStatus = mpu.dmpInitialize(); // 初始化 DMP
    if (devStatus ==0) {
        mpu.setDMPEnabled(true); // 启用 DMP
    } else {
        Serial.println("DMP 初始化失败! ");
        while (1); 
    }

    // 配置 MPU6050 的参数
    mpu.setRate(39);         // 设置采样率为200Hz，匹配DMP
    mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250); // 设置陀螺仪量程为±250度/秒
    mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2); // 设置加速度量程为±2g

}

bool MPU6050Handler::readData(float &accelX, float &accelY, float &accelZ,
                              float &gyroX, float &gyroY, float &gyroZ,
                              float &qW, float &qX, float &qY, float &qZ) {
    uint16_t fifoCount = mpu.getFIFOCount(); // 获取 FIFO 缓冲区的字节数
    if (fifoCount >= mpu.dmpGetFIFOPacketSize()) { // 检查FIFO 缓冲区是否有一个完整数据包
        uint8_t fifoBuffer[64]; 
        mpu.getFIFOBytes(fifoBuffer, mpu.dmpGetFIFOPacketSize());
        
        Quaternion q;
        VectorInt16 accel,gyro;
        
        mpu.dmpGetQuaternion(&q, fifoBuffer); 
        mpu.dmpGetAccel(&accel, fifoBuffer);
        mpu.dmpGetGyro(&gyro, fifoBuffer);

        // 将数据转换为标准单位
        accelX = accel.x / 16384.0f; // 加速度计数据（单位：g）
        accelY = accel.y / 16384.0f;
        accelZ = accel.z / 16384.0f;
        gyroX = gyro.x / 131.0f;    // 陀螺仪数据（单位：°/s）
        gyroY = gyro.y / 131.0f;
        gyroZ = gyro.z / 131.0f;

        // 提取四元数分量
        qW = q.w;
        qX = q.x;
        qY = q.y;
        qZ = q.z;
        
        return true; // 成功读取数据
    }
    return false; // 没有足够的数据包
}