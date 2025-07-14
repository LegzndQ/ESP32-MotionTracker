#include "config.h"

// 电机控制参数
const int pwmStep = 10;
const int pwmForward = 150; // 前进时的 PWM 值

// Hall 传感器参数
const int pulsesPerRevolution = 7;
const int gearRatio = 10; // 齿轮比

// 系统参数
const int hallUpdateInterval = 10;