/*
* 
*/

#include "config.h"
#include "InfraredReceiver.h"
#include "MotorControl.h"

// 全局变量
bool collectingData = false;

// 全局对象
MotorControl motorControl;
IRControl irControl(IR_RECV_PIN, &motorControl, &collectingData);

void setup() {
    Serial.begin(115200);
    motorControl.init();
    irControl.init();
}

void loop() {
    if (collectingData) {
        Serial.println("收到红外信号");
    }

    irControl.processCommand();

    delay(50);
}