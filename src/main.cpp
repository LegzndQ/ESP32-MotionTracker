/**
 * 无任何滤波版本
 * 采样率为100Hz，可调
 * 上电就一直输出数据
 * 可Wifi通信连接Python上位机 test
 */

#include <WiFi.h>
#include <Wire.h>
#include <MPU6050.h>

const char* ssid = "your_SSID"; // 替换为你的WiFi SSID
const char* password = "your_PASSWORD"; // 替换为你的WiFi密码
const IPAddress remoteIP(192, 168, 1, 101); // 上位机的 IP 地址
const int udpPort = 12345; // 上位机的 UDP 端口

WiFiUDP udp;
MPU6050 mpu;

/**
 * @brief 发送数据到上位机
 * @param accelX 加速度 X 轴数据
 * ...
 */
void sendData(float accelX, float accelY, float accelZ, 
			  float gyroX, float gyroY, float gyroZ) {
	char buffer[128];
	snprintf(buffer, sizeof(buffer), "%ld,%f,%f,%f,%f,%f,%f",
			 millis(), accelX, accelY, accelZ, gyroX, gyroY, gyroZ);
	udp.beginPacket(remoteIP, udpPort);
	udp.print(buffer);
	udp.endPacket();
}

void setup() {
	Serial.begin(115200); // 初始化串口通信
    Wire.begin(21, 20); // 初始化I2C总线，SDA、SCL引脚
    mpu.initialize(); // 初始化MPU6050

    // 配置 MPU6050 的参数
    mpu.setRate(79);                  // 设置采样率为100Hz
    //mpu.setDLPFMode(3);                 // 设置数字低通滤波器截止频率为44Hz
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
    // 定期读取 MPU6050 的数据
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz); // 获取加速度和陀螺仪数据

	// 将原始数据转换为标准单位
	float accelX = ax / 16384.0f; // 加速度计数据（单位：g）
	float accelY = ay / 16384.0f;
	float accelZ = az / 16384.0f;
	float gyroX = gx / 131.0f;    // 陀螺仪数据（单位：°/s）
	float gyroY = gy / 131.0f;
	float gyroZ = gz / 131.0f;

    sendData(accelX, accelY, accelZ, 
		gyroX, gyroY, gyroZ); // 发送数据到上位机
    
		delay(10); // 每 10ms 发送一次数据（对应 100Hz）
}