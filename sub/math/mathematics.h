#ifndef MATHEMATICS_H
#define MATHEMATICS_H

#include <stdint.h>
#include <math.h>

namespace Mathematics
{
    const int32_t DegreeAccuracy = 10000000;    ///< Точность измерения координат: количество знаков после запятой для градуса
    const uint16_t DegreesInPI = 180;           ///< Количество градусов для числа Пи, град
    const uint16_t DegreesInFullCircle = 360;   ///< Количество градусов в окружности, град
    const uint64_t EquatorLength = 4000000000;  ///< Длина Экваториальной линии, см
    const float GravityAcceleration = 980.665;  ///< Ускорение свободного падения, см/с^2

    struct Cordinates
    {
        int32_t lattitude;  ///< Широта
        int32_t longitude;  ///< Долгота
    };

    struct Point
    {
        int32_t x;  ///< Расстояние по долготе (запад-восток), см
        int32_t y;  ///< Расстояние по широте (север-юг), см
    };

    /// @brief Преобразование градусов в радианы
    /// @param degree - угол, град
    double DegreeToRadian(double degree);

    /// @brief Преобразование радиан в градусы
    /// @param radian - угол, радианы
    double RadianToDegree(double radian);

    /// @brief Преобразование координат в метры
    /// @param crd - координаты
    Point CordinateToPoint(const Cordinates &crd);

    /// @brief Преобразование точки в километрах относительно экватора и нулевого меридиана
    /// @param lttd - широта, для которой считать окружность вокруг Земли
    /// @param pnt - точка (широта и долгота) для преобразования
    Cordinates PointToCordinate(const int32_t &lttd, const Point &pnt);

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
    /// @param xsp - скорость по долготе (запад-восток), см/с
    /// @param ysp - скорость по широте (север-юг), см/с
    /// @param xacc - ускорение по долготе (запад-восток), см/с^2
    /// @param yacc - ускорение по широте (север-юг), см/с^2
    /// @param ms - время, мс
    Point FindDelta(float xsp, float ysp, float xacc, float yacc, uint16_t ms);

    /// @brief Расчёт позиции новой точки
    /// @param x - широта, см
    /// @param y - долгота, см
    /// @param z - высота над поверхностью земли, см
    /// @param xsp - скорость по долготе (запад-восток), см/с
    /// @param ysp - скорость по широте (север-юг), см/с
    /// @param zsp - скорость снижения, см/с
    /// @param xacc - ускорение по долготе (запад-восток), см/с^2
    /// @param yacc - ускорение по широте (север-юг), см/с^2
    /// @param zacc - ускорение по высоте, см/с^2
    Point FindNewPosition(int32_t x, int32_t y, int32_t z, float xsp, float ysp, float zsp, float xacc, float yacc, float zacc);
};

#endif // MATHEMATICS_H
