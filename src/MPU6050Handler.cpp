#include "MPU6050Handler.h"
#include <Wire.h>
#include "config.h"

MPU6050Handler::MPU6050Handler() : mpu(MPU6050_DEFAULT_ADDRESS) {}

void MPU6050Handler::init() {
    Wire.begin(I2C_SDA, I2C_SCL); // 初始化I2C总线，SDA、SCL引脚
    mpu.initialize(); // 初始化MPU6050

	uint8_t devStatus = mpu.dmpInitialize(); // 初始化 DMP
    if (devStatus ==0) {
        mpu.setDMPEnabled(true); // 启用 DMP
    } else {
        Serial.println("DMP 初始化失败! ");
        while (1); 
    }

    // 配置 MPU6050 的参数
    mpu.setRate(39);         // 设置采样率为200Hz，匹配dmp处理速度
    mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250); // 设置陀螺仪量程为±250度/秒
    mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2); // 设置加速度量程为±2g

}

MPUData MPU6050Handler::readData() {
    MPUData data;

    uint16_t fifoCount = mpu.getFIFOCount(); // 获取 FIFO 缓冲区的字节数
    
    if (fifoCount >= mpu.dmpGetFIFOPacketSize()) { // 检查FIFO 缓冲区是否有一个完整数据包
        // 读取数据并设置 valid = true
        data.valid = true;
        
        uint8_t fifoBuffer[64]; 
        mpu.getFIFOBytes(fifoBuffer, mpu.dmpGetFIFOPacketSize());
        
        Quaternion q;
        VectorInt16 accel,gyro;
        
        mpu.dmpGetQuaternion(&q, fifoBuffer); 
        mpu.dmpGetAccel(&accel, fifoBuffer);
        mpu.dmpGetGyro(&gyro, fifoBuffer);

        // 将数据转换为标准单位
        data.accelX = accel.x / 16384.0f; // 加速度计数据（单位：g）
        data.accelY = accel.y / 16384.0f;
        data.accelZ = accel.z / 16384.0f;
        data.gyroX = gyro.x / 131.0f;    // 陀螺仪数据（单位：°/s）
        data.gyroY = gyro.y / 131.0f;
        data.gyroZ = gyro.z / 131.0f;

        // 提取四元数分量
        data.qW = q.w;
        data.qX = q.x;
        data.qY = q.y;
        data.qZ = q.z;
        
        return data; 
    }
    data.valid = false; 
    return data; // 没有足够的数据包
}