#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

// 引脚定义 
#define IR_RECV_PIN 19 // 红外接收端口
#define MOTOR_A_PWM1 47
#define MOTOR_A_PWM2 48
#define MOTOR_B_PWM1 38
#define MOTOR_B_PWM2 39
#define HALL_A1 3
#define HALL_A2 46
#define HALL_B1 40
#define HALL_B2 41
#define MPU_INT_PIN 12 // MPU6050 中断引脚连接到 ESP32 的 IO12

// 电机控制参数
extern const int pwmStep; // 电机 PWM 步进值
extern const int pwmForward; // 电机前进 PWM 值

// 红外遥控指令码
constexpr uint64_t IR_LEFT_MOTOR_DECREASE = 0xFF30CF;
constexpr uint64_t IR_LEFT_MOTOR_INCREASE = 0xFF10EF;
constexpr uint64_t IR_RIGHT_MOTOR_DECREASE = 0xFF7A85;
constexpr uint64_t IR_RIGHT_MOTOR_INCREASE = 0xFF5AA5;
constexpr uint64_t IR_STOP = 0xFFA25D;
constexpr uint64_t IR_FORWARD = 0xFF6897;

#endif