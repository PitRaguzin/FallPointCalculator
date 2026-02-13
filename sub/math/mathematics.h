#ifndef MATHEMATICS_H
#define MATHEMATICS_H

#include <stdint.h>
#include <math.h>

namespace Mathematics
{
    const int32_t DegreeAccuracy = 10000000;        ///< Точность измерения координат: количество знаков после запятой для градуса
    const uint16_t DegreesInPI = 180;               ///< Количество градусов для числа Пи, град
    const uint16_t DegreesInFullCircle = 360;       ///< Количество градусов в окружности, град
    const uint64_t EquatorLength = 4000000000;      ///< Длина Экваториальной линии, см
    const float GravityAcceleration = 980.665;      ///< Ускорение свободного падения, см/с^2

    const double a = 637813700.0;                   ///< Большая полуось Земли, см
    const double b = 635675231.42;                  ///< Малая полуось Земли, см
    const double f = (a - b) / a;                   ///< Геометрическое сжатие
    const double esqw = 1 - ((b * b) / (a * a));    ///< Квадрат первого эксцентриситета Земли
    const double e2sqw = ((a * a) / (b * b)) - 1;   ///< Квадрат второго эксцентриситета Земли

    struct GeographicPoint
    {
        int32_t lattitude;  ///< Широта, град
        int32_t longitude;  ///< Долгота, град
        int32_t altitude;   ///< Высота, см
    };

    struct Point
    {
        int32_t x;  ///< Для планарной системы расстояние по долготе (запад-восток), см
        int32_t y;  ///< Для планарной системы расстояние по широте (север-юг), см
        int32_t z;  ///< Используется только в геоцентрической системе: высота WGS84, см
        int32_t dz; ///< Используется только в геоцентрической системе: высота поверхности относительно уровня моря, см
    };

    /// @brief Преобразование градусов в радианы
    /// @param degree - угол, град
    double DegreeToRadian(double degree);

    /// @brief Преобразование радиан в градусы
    /// @param radian - угол, радианы
    double RadianToDegree(double radian);

    /// @brief Преобразование координат из географических (WGS84) в проекционные
    /// @param crd - координаты
    Point WGS84ToProjection(const GeographicPoint &crd);

    /// @brief Преобразование координат из проекционных в географические (WGS84)
    /// @param lttd - широта, для которой считать окружность вокруг Земли
    /// @param pnt - точка (широта и долгота) для преобразования
    GeographicPoint ProjectionToWGS84(const int32_t &lttd, const Point &pnt);

    /// @brief Преобразование координат из географических (WGS84) в геоцентрические
    /// @param gp - точка (широта и долгота) для преобразования
    Point WGS84ToXYZ(const GeographicPoint &gp);

    /// @brief Преобразование координат из геоцентрических в географические (WGS84)
    /// @param gp - точка (широта и долгота) для преобразования
    GeographicPoint XYZToWGS84(const Point &p);

    /// @brief Подсчёт параметра, изменяющегося во времени (скорость\ускорение)
    /// @param val1 - начальное значение
    /// @param val2 - конечное значение
    /// @param ms - время, мс
    double CountTimeChangeParam(double val1, double val2, uint64_t ms);

    /// @brief Расчёт времени до падения
    /// @param alt - высота над поверхностью земли, см
    /// @param zsp - скорость снижения, см/с
    /// @param zacc - ускорение, см/с^2
    uint16_t MillisecondsToFall(int32_t alt, float zsp, float zacc);

    /// @brief Расчёт перемещения согласно скоростям по широте и долготе и времени
    /// @param xsp - скорость по координате x, см/с
    /// @param ysp - скорость по координате y, см/с
    /// @param zsp - скорость по координате z, см/с
    /// @param xacc - ускорение по координате x, см/с^2
    /// @param yacc - ускорение по координате y, см/с^2
    /// @param zacc - ускорение по координате z, см/с^2
    /// @param ms - время, мс
    Point FindDelta(float xsp, float ysp, float zsp, float xacc, float yacc, float zacc, uint16_t ms);

    /// @brief Расчёт позиции новой точки
    /// @param x - координата x в геоцентрической системе, см
    /// @param y - координата y в геоцентрической системе, см
    /// @param z - координата z в геоцентрической системе, см
    /// @param h - высота над поверхностью земли, см
    /// @param xsp - скорость по координате x, см/с
    /// @param ysp - скорость по координате y, см/с
    /// @param zsp - скорость по координате z, см/с
    /// @param zsp - скорость снижения, см/с
    /// @param xacc - ускорение по координате x, см/с^2
    /// @param yacc - ускорение по координате y, см/с^2
    /// @param zacc - ускорение по координате z, см/с^2
    /// @param hacc - ускорение снижения над поверхностью Земли, см/с^2
    Point FindNewPosition(int32_t x, int32_t y, int32_t z, int32_t h, float xsp, float ysp, float zsp, float hsp, float xacc, float yacc, float zacc, float hacc);
};

#endif // MATHEMATICS_H
