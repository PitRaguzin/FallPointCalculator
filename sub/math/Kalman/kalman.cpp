#include "kalman.h"

Kalman::Kalman(double dt)
{
    x = VectorXd::Zero(6);                  // Вектор состояния: [x, y, z, vx, vy, vz]
    F = MatrixXd::Identity(6, 6);
        setPredictionPeriod(dt);             // Матрица перехода состояния F
    H = MatrixXd::Zero(3, 6);
        H(0, 0) = 1;
        H(1, 1) = 1;
        H(2, 2) = 1;                        // Матрица измерений H (измеряем только позицию x, y, z)
    P = MatrixXd::Identity(6, 6) * 1000;    // Ковариация ошибки P
    Q = MatrixXd::Identity(6, 6) * 0.1;     // Матрица шума процесса Q
    R = MatrixXd::Identity(3, 3) * 0.1;     // Матрица шума измерений R
}

void Kalman::predict()
{
    x = F * x;
    P = F * P * F.transpose() + Q;
}

void Kalman::update(const Vector3d& measurement)
{
    VectorXd y = measurement - H * x; // Ошибка измерения
    MatrixXd S = H * P * H.transpose() + R;
    MatrixXd K = P * H.transpose() * S.inverse(); // Усиление Калмана

    x = x + K * y;
    P = (MatrixXd::Identity(6, 6) - K * H) * P;
}

Vector3d Kalman::getState()
{
    return x.head<3>();
}

void Kalman::setPredictionPeriod(double dt)
{
    F(0, 3) = dt;
    F(1, 4) = dt;
    F(2, 5) = dt;
}
