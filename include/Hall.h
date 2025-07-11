#ifndef HALL_H
#define HALL_H

#include <Arduino.h>

struct HallData {
    float motorASpeed;
    float motorBSpeed;
    unsigned long timestamp;
};

class Hall {
private:
    volatile unsigned long hallA1Count;
    volatile unsigned long hallA2Count;
    volatile unsigned long hallB1Count;
    volatile unsigned long hallB2Count;

    unsigned long lastHallTime;

public:
    Hall();
    void init();
    HallData readData();
    void resetCounts();
    float calculateMotorSpeed(unsigned long pulseCount, unsigned long timeDiff);

    // 中断处理函数（需要声明为静态成员）
    static void IRAM_ATTR onHallA1Pulse();
    static void IRAM_ATTR onHallA2Pulse();
    static void IRAM_ATTR onHallB1Pulse();
    static void IRAM_ATTR onHallB2Pulse();

    // 设置中断标志的公共方法
    void incrementHallA1();
    void incrementHallA2();
    void incrementHallB1();
    void incrementHallB2();
};

// 全局传感器实例指针，用于中断回调
extern Hall* g_hall;

#endif