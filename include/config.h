#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

// 引脚定义 
#define IR_RECV_PIN 19 // 红外接收端口
#define MOTOR_A_PWM1 38
#define MOTOR_A_PWM2 39
#define MOTOR_B_PWM1 47
#define MOTOR_B_PWM2 48
#define HALL_A1 40
#define HALL_A2 41
#define HALL_B1 3
#define HALL_B2 46
#define MPU_INT_PIN 12 // MPU6050 中断引脚连接到 ESP32 的 IO12

// I2C 引脚
#define I2C_SDA 21
#define I2C_SCL 20

// WiFi配置 - 请在config.h中定义这些值
#ifndef WIFI_SSID
#define WIFI_SSID "your_SSID"  // 默认值，请修改为实际WiFi名称
#endif

#ifndef WIFI_PASSWORD  
#define WIFI_PASSWORD "your_PASSWORD"  // 默认值，请修改为实际WiFi密码
#endif

// 电机控制参数
extern const int pwmStep; // 电机 PWM 步进值
extern const int pwmForward; // 电机前进 PWM 值

// Hall 传感器参数
extern const int pulsesPerRevolution;
extern const int gearRatio;

// 系统参数
extern const int hallUpdateInterval; // 霍尔传感器更新间隔（毫秒）

// 红外遥控指令码
constexpr uint64_t IR_LEFT_MOTOR_DECREASE = 0xFF10EF; // 4
constexpr uint64_t IR_LEFT_MOTOR_INCREASE = 0xFF30CF; // 1
constexpr uint64_t IR_RIGHT_MOTOR_DECREASE = 0xFF5AA5; // 6
constexpr uint64_t IR_RIGHT_MOTOR_INCREASE = 0xFF7A85; //3
constexpr uint64_t IR_STOP = 0xFFA25D;
constexpr uint64_t IR_FORWARD = 0xFF6897;

#endif