#include "InfraredReceiver.h"
#include "config.h"

IRControl::IRControl(int pin, MotorControl* motor, bool* dataFlag)
    : irrecv(pin), motorControl(motor), collectingDataFlag(dataFlag) {}

void IRControl::init() {
    irrecv.enableIRIn();
    Serial.println("IR 接收器已启动");
}

bool IRControl::processCommand() {
    if (irrecv.decode(&results)) {
        uint64_t value = results.value;
        Serial.printf("接收到的红外信号: 0x%llX\n", value);

        IRCommand cmd = decodeIRValue(value);
        executeCommand(cmd);

        irrecv.resume();  // 继续接收下一个信号
        return true;
    }
    return false;
}

IRCommand IRControl::decodeIRValue(uint64_t value) {
    switch (value) {
        case IR_LEFT_MOTOR_DECREASE:
            return CMD_LEFT_NOTOR_DECREASE;
        case IR_LEFT_MOTOR_INCREASE:
            return CMD_LEFT_MOTOR_INCREASE;
        case IR_RIGHT_MOTOR_DECREASE:
            return CMD_RIGHT_MOTOR_DECREASE;
        case IR_RIGHT_MOTOR_INCREASE:
            return CMD_RIGHT_MOTOR_INCREASE;
        case IR_STOP:
            return CMD_STOP;
        case IR_FORWARD:
            return CMD_FORWARD;
        default:
            return CMD_UNKNOWN;  // 未知指令
    }
}

void IRControl::executeCommand(IRCommand cmd) {
    switch (cmd) {
        case CMD_LEFT_NOTOR_DECREASE:
            *collectingDataFlag = true;
            motorControl->adjustMotorA(-pwmStep);
            break;
        case CMD_LEFT_MOTOR_INCREASE:
            *collectingDataFlag = true;
            motorControl->adjustMotorA(pwmStep);
            break;
        case CMD_RIGHT_MOTOR_DECREASE:
            *collectingDataFlag = true;     
            motorControl->adjustMotorB(-pwmStep);
            break;
        case CMD_RIGHT_MOTOR_INCREASE:
            *collectingDataFlag = true;
            motorControl->adjustMotorB(pwmStep);
            break;
        case CMD_STOP:
            *collectingDataFlag = false;
            motorControl->stopCar();
            break;
        case CMD_FORWARD:
            *collectingDataFlag = true;
            motorControl->setForward(pwmForward);
            break;
        case CMD_UNKNOWN:
            Serial.println("未知指令"); // 便于拓展
            break;
        default:
            Serial.println("未知指令");
            break;
    }    

}