#include "config.h"

// WiFi 配置
const char* ssid = "Legznd";
const char* password = "12345678";
const char* serverIP = "172.20.10.11";
const int udpPort = 12345;

// 电机控制参数
const int pwmStep = 10;

// Hall 传感器参数
const int pulsesPerRevolution = 7;
const int gearRatio = 30;

// 卡尔曼滤波器参数
const float Q_angle_default = 0.001f;
const float Q_bias_default = 0.003f;
const float R_measure_default = 0.03f;

// 系统参数
const int hallUpdateInterval = 50;
const int mainLoopDelay = 50;

// 红外遥控指令码
const uint64_t IR_LEFT_MOTOR_DECREASE = 0xFF30CF;
const uint64_t IR_LEFT_MOTOR_INCREASE = 0xFF10EF;
const uint64_t IR_RIGHT_MOTOR_DECREASE = 0xFF7A85;
const uint64_t IR_RIGHT_MOTOR_INCREASE = 0xFF5AA5;
const uint64_t IR_STOP = 0xFFA25D;
const uint64_t IR_FORWARD = 0xFF6897;