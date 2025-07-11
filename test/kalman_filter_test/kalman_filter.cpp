#include "kalman_filter.h"
#include "config.h"

KalmanFilterManager::KalmanFilterManager() {}

void KalmanFilterManager::init() {
    initKalmanFilter(kalmanAccelX);
    initKalmanFilter(kalmanAccelY);
    initKalmanFilter(kalmanAccelZ);
    initKalmanFilter(kalmanGyroX);
    initKalmanFilter(kalmanGyroY);
    initKalmanFilter(kalmanGyroZ);
}

void KalmanFilterManager::initKalmanFilter(KalmanFilter &kf) {
    kf.Q_angle = Q_angle_default;
    kf.Q_bias = Q_bias_default;
    kf.R_measure = R_measure_default;

    kf.angle = 0.0f;
    kf.bias = 0.0f;
    kf.P[0][0] = 0.0f;
    kf.P[0][1] = 0.0f;
    kf.P[1][0] = 0.0f;
    kf.P[1][1] = 0.0f;
}

float KalmanFilterManager::updateKalmanFilter(KalmanFilter &kf, float newAngle, float newRate, float dt) {
    // 预测阶段
    kf.rate = newRate - kf.bias;
    kf.angle += dt * kf.rate;

    kf.P[0][0] += dt * (dt * kf.P[1][1] - kf.P[0][1] - kf.P[1][0] + kf.Q_angle);
    kf.P[0][1] -= dt * kf.P[1][1];
    kf.P[1][0] -= dt * kf.P[1][1];
    kf.P[1][1] += kf.Q_bias * dt;

    // 更新阶段
    float S = kf.P[0][0] + kf.R_measure; // 估计误差
    float K[2]; // 卡尔曼增益
    K[0] = kf.P[0][0] / S;
    K[1] = kf.P[1][0] / S;

    float y = newAngle - kf.angle; // 角度偏差
    kf.angle += K[0] * y;
    kf.bias += K[1] * y;

    float P00_temp = kf.P[0][0];
    float P01_temp = kf.P[0][1];

    kf.P[0][0] -= K[0] * P00_temp;
    kf.P[0][1] -= K[0] * P01_temp;
    kf.P[1][0] -= K[1] * P00_temp;
    kf.P[1][1] -= K[1] * P01_temp;

    return kf.angle;
}

void KalmanFilterManager::filterSensorData(float accelX, float accelY, float accelZ,
                                          float gyroX, float gyroY, float gyroZ,
                                          float dt,
                                          float &filteredAccelX, float &filteredAccelY, float &filteredAccelZ,
                                          float &filteredGyroX, float &filteredGyroY, float &filteredGyroZ) {
    filteredAccelX = updateKalmanFilter(kalmanAccelX, accelX, gyroX, dt);
    filteredAccelY = updateKalmanFilter(kalmanAccelY, accelY, gyroY, dt);
    filteredAccelZ = updateKalmanFilter(kalmanAccelZ, accelZ, gyroZ, dt);
    filteredGyroX = updateKalmanFilter(kalmanGyroX, accelX, gyroX, dt);
    filteredGyroY = updateKalmanFilter(kalmanGyroY, accelY, gyroY, dt);
    filteredGyroZ = updateKalmanFilter(kalmanGyroZ, accelZ, gyroZ, dt);
}