#ifndef KALMAN_FILTER_H
#define KALMAN_FILTER_H

struct KalmanFilter {
    float Q_angle;   // 过程噪声协方差（角度）
    float Q_bias;    // 过程噪声协方差（偏差）
    float R_measure; // 测量噪声协方差

    float angle; // 估计的角度
    float bias;  // 估计的偏差
    float rate;  // 未校正的角速度

    float P[2][2]; // 误差协方差矩阵
};

class KalmanFilterManager {
private:
    KalmanFilter kalmanAccelX, kalmanAccelY, kalmanAccelZ;
    KalmanFilter kalmanGyroX, kalmanGyroY, kalmanGyroZ;

public:
    KalmanFilterManager();
    void init();
    void initKalmanFilter(KalmanFilter &kf);
    float updateKalmanFilter(KalmanFilter &kf, float newAngle, float newRate, float dt);
    
    // 滤波传感器数据
    void filterSensorData(float accelX, float accelY, float accelZ,
                         float gyroX, float gyroY, float gyroZ,
                         float dt,
                         float &filteredAccelX, float &filteredAccelY, float &filteredAccelZ,
                         float &filteredGyroX, float &filteredGyroY, float &filteredGyroZ);
};

#endif