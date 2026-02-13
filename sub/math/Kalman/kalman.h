#ifndef KALMAN_H
#define KALMAN_H

#include "../Eigen/Dense"

using namespace Eigen;

class Kalman
{
public:
    /// @param dt - период времени с которым приходят данные
    Kalman(double dt);

    /// @brief Прогноз следующего состояния
    void predict();

    /// @brief Коррекция предсказанного значения на основе новых значений
    void update(const Vector3d& measurement);

    /// @brief Вектор положения
    Vector3d getState();

    /// @brief Задание времени для прогноза следующего значения
    /// @param dt - период времени с которым приходят данные
    void setPredictionPeriod(double dt);

private:
    VectorXd x; ///< Состояние
    MatrixXd F, ///< Матрица перехода состояния
             H, ///< Матрица измерений
             P, ///< Ковариационная матрица ошибки
             Q, ///< Матрица шума процесса
             R; ///< Матрица шума измерений
};

#endif // KALMAN_H
