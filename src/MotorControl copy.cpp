#include "MotorControl.h"
#include "config.h"

MotorControl::MotorControl() : motorAPWM(0), motorBPWM(0) {}

void MotorControl::init() {
    pinMode(MOTOR_A_PWM1, OUTPUT);
    pinMode(MOTOR_A_PWM2, OUTPUT);
    pinMode(MOTOR_B_PWM1, OUTPUT);
    pinMode(MOTOR_B_PWM2, OUTPUT);
}

void MotorControl::setMotorPWM(int pwmA, int pwmB) {
    motorAPWM = pwmA;
    motorBPWM = pwmB;

    if (pwmA >= 0) {
        analogWrite(MOTOR_A_PWM1, pwmA);
        analogWrite(MOTOR_A_PWM2, 0);
    } else {
        analogWrite(MOTOR_A_PWM1, 0);
        analogWrite(MOTOR_A_PWM2, -pwmA);
    }

    if (pwmB >= 0) {
        analogWrite(MOTOR_B_PWM1, pwmB);
        analogWrite(MOTOR_B_PWM2, 0);
    } else {
        analogWrite(MOTOR_B_PWM1, 0);
        analogWrite(MOTOR_B_PWM2, -pwmB);
    }
}

void MotorControl::stopCar() {
    setMotorPWM(0, 0);
}

void MotorControl::adjustMotorA(int increment) {
    motorAPWM += increment;
    setMotorPWM(motorAPWM, motorBPWM);
}

void MotorControl::adjustMotorB(int increment) {
    motorBPWM += increment;
    setMotorPWM(motorAPWM, motorBPWM);
}

void MotorControl::setForward(int pwm) {
    motorAPWM = pwm;
    motorBPWM = pwm;
    setMotorPWM(-motorAPWM, -motorBPWM);
}

int MotorControl::getMotorAPWM() const {
    return motorAPWM;
}

int MotorControl::getMotorBPWM() const {
    return motorBPWM;
}
