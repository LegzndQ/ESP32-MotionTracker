#ifndef INFRARED_RECEIVER_H
#define INFRARED_RECEIVER_H

#include <IRrecv.h>
#include <IRutils.h>
#include <IRremoteESP8266.h>
#include "MotorControl.h"

enum IRCommand {
    CMD_LEFT_NOTOR_DECREASE,
    CMD_LEFT_MOTOR_INCREASE,
    CMD_RIGHT_MOTOR_DECREASE,
    CMD_RIGHT_MOTOR_INCREASE,
    CMD_STOP,
    CMD_FORWARD,
    CMD_UNKNOWN
};

class IRControl {
private:
    IRrecv irrecv;
    decode_results results;
    MotorControl* motorControl;
    bool* collectingDataFlag;

public:
    IRControl(int pin, MotorControl* motor, bool* dataFlag);
    void init();
    bool processCommand();
    IRCommand decodeIRValue(uint64_t value);
    void executeCommand(IRCommand cmd);
};

#endif