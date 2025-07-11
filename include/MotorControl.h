#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <Arduino.h>

class MotorControl {
private:
    int motorAPWM;
    int motorBPWM;

public:
    MotorControl();
    void init();
    void setMotorPWM(int pwmA, int pwmB);
    void stopCar();
    void adjustMotorA(int increment);
    void adjustMotorB(int increment);
    void setForward(int pwm);
    int getMotorAPWM() const;
    int getMotorBPWM() const;
    
};

#endif