#ifndef CONFIG_H
#define CONFIG_H

// 引脚定义
#define IR_RECV_PIN 19
#define MOTOR_A_PWM1 47
#define MOTOR_A_PWM2 48
#define MOTOR_B_PWM1 38
#define MOTOR_B_PWM2 39
#define HALL_A1 3
#define HALL_A2 46
#define HALL_B1 40
#define HALL_B2 41
#define MPU_INT_PIN 12

// I2C 引脚
#define I2C_SDA 21
#define I2C_SCL 20

// WiFi 配置
extern const char* ssid;
extern const char* password;
extern const char* serverIP;
extern const int udpPort;

// 电机控制参数
extern const int pwmStep;

// Hall 传感器参数
extern const int pulsesPerRevolution;
extern const int gearRatio;

// 卡尔曼滤波器参数
extern const float Q_angle_default;
extern const float Q_bias_default;
extern const float R_measure_default;

// 系统参数
extern const int hallUpdateInterval;
extern const int mainLoopDelay;

// 红外遥控指令码
extern const uint64_t IR_LEFT_MOTOR_DECREASE;
extern const uint64_t IR_LEFT_MOTOR_INCREASE;
extern const uint64_t IR_RIGHT_MOTOR_DECREASE;
extern const uint64_t IR_RIGHT_MOTOR_INCREASE;
extern const uint64_t IR_STOP;
extern const uint64_t IR_FORWARD;

#endif