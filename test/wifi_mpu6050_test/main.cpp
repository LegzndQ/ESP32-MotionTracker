/**
 * V1.1 加速度和陀螺仪经过dmp处理版本，可与原始版本进行对比
 * 采样率为200Hz，匹配dmp采样
 * 上电就一直输出数据
 * 可Wifi通信连接Python上位机
 */

#include <WiFi.h>
#include <Wire.h>
#include <MPU6050_6Axis_MotionApps20.h>

// WiFi配置 - 请在config.h中定义这些值
#ifndef WIFI_SSID
#define WIFI_SSID "your_SSID"  // 默认值，请修改为实际WiFi名称
#endif

#ifndef WIFI_PASSWORD  
#define WIFI_PASSWORD "your_PASSWORD"  // 默认值，请修改为实际WiFi密码
#endif

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const IPAddress remoteIP(192, 168, 1, 101); // 上位机的 IP 地址
const int udpPort = 12345; // 上位机的 UDP 端口

WiFiUDP udp;
MPU6050 mpu;

// 四元数
float q_w = 1.0f, q_x = 0.0f, q_y = 0.0f, q_z = 0.0f;

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
void sendData(float accelX, float accelY, float accelZ, 
              float gyroX, float gyroY, float gyroZ,
              float q_w, float q_x, float q_y, float q_z) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%ld,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f",
             millis(), accelX, accelY, accelZ, gyroX, gyroY, gyroZ, q_w, q_x, q_y, q_z);
    udp.beginPacket(remoteIP, udpPort);
    udp.print(buffer);
    udp.endPacket();
}

void setup() {
    Serial.begin(115200); // 初始化串口通信
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

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi 连接成功");

    udp.begin(udpPort); // 初始化 UDP
}

void loop() {

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
        float accelX = accel.x / 16384.0f; // 加速度计数据（单位：g）
        float accelY = accel.y / 16384.0f;
        float accelZ = accel.z / 16384.0f;
        float gyroX = gyro.x / 131.0f;    // 陀螺仪数据（单位：°/s）
        float gyroY = gyro.y / 131.0f;
        float gyroZ = gyro.z / 131.0f;

         // 提取四元数分量
         q_w = q.w;
         q_x = q.x;
         q_y = q.y;
         q_z = q.z;

        // 发送数据到上位机
        sendData(accelX, accelY, accelZ, 
            gyroX, gyroY, gyroZ, 
            q_w, q_x, q_y, q_z);

    } else {
        Serial.println("FIFO 数据不足！ ");
    }
       
    delay(10); // 每 10ms 发送一次数据（对应 100Hz）
    
}