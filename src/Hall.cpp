#include "Hall.h"
#include "config.h"

// 全局霍尔传感器实例指针
Hall* g_hall = nullptr;

Hall::Hall() : hallA1Count(0), hallA2Count(0), hallB1Count(0), hallB2Count(0), lastHallTime(0) {
    g_hall = this;
}

void Hall::init() {
    // 配置 Hall 传感器引脚
    pinMode(HALL_A1, INPUT_PULLUP);
    pinMode(HALL_A2, INPUT_PULLUP);
    pinMode(HALL_B1, INPUT_PULLUP);
    pinMode(HALL_B2, INPUT_PULLUP);
    
    // 配置 Hall 传感器中断
    attachInterrupt(digitalPinToInterrupt(HALL_A1), onHallA1Pulse, RISING);
    attachInterrupt(digitalPinToInterrupt(HALL_A2), onHallA2Pulse, RISING);
    attachInterrupt(digitalPinToInterrupt(HALL_B1), onHallB1Pulse, RISING);
    attachInterrupt(digitalPinToInterrupt(HALL_B2), onHallB2Pulse, RISING);

}

HallData Hall::readData() {
    HallData data;

    unsigned long currentTime = millis();
    unsigned long timeDiff = currentTime - lastHallTime;

    // 每隔 hallUpdateInterval 毫秒计算一次电机角速度
    if (timeDiff >= hallUpdateInterval) {
        data.motorASpeed = calculateMotorSpeed(hallA1Count, timeDiff);
        data.motorBSpeed = calculateMotorSpeed(hallB1Count, timeDiff);
        resetCounts();
        lastHallTime = currentTime;
    } else {
        data.motorASpeed = 0;
        data.motorBSpeed = 0;
    }

    data.timestamp = currentTime;
    return data;
}

void Hall::resetCounts() {
    hallA1Count = 0;
    hallA2Count = 0;
    hallB1Count = 0;
    hallB2Count = 0;
}

float Hall::calculateMotorSpeed(unsigned long pulseCount, unsigned long timeDiff) {
    // if (timeDiff == 0) return 0.0f; // 防止除以零
    return pulseCount * (360.0 / pulsesPerRevolution / timeDiff * 1000 / gearRatio);
}

// 静态中断处理函数
void IRAM_ATTR Hall::onHallA1Pulse() {
    if (g_hall) {
        g_hall->incrementHallA1();
    }
}

void IRAM_ATTR Hall::onHallA2Pulse() {
    if (g_hall) {
        g_hall->incrementHallA2();
    }
}

void IRAM_ATTR Hall::onHallB1Pulse() {
    if (g_hall) {
        g_hall->incrementHallB1();
    }
}

void IRAM_ATTR Hall::onHallB2Pulse() {
    if (g_hall) {
        g_hall->incrementHallB2();
    }
}

// 公共方法用于设置中断标志
void Hall::incrementHallA1() {
    hallA1Count++;
}

void Hall::incrementHallA2() {
    hallA2Count++;
}

void Hall::incrementHallB1() {
    hallB1Count++;
}

void Hall::incrementHallB2() {
    hallB2Count++;
}
