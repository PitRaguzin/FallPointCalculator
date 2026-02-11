#include "mathematics.h"

namespace Mathematics
{
    double DegreeToRadian(double degree)
    {
        return (degree / DegreesInPI) * M_PI;
    }

    double RadianToDegree(double radian)
    {
        return (radian / M_PI) * DegreesInPI;
    }

    Point CordinateToPoint(const Cordinates &crd)
    {
        Point res;

        double cm_on_grad_y = EquatorLength / DegreesInFullCircle;                          // Расстояние в одном градусе широты, см
        double grad_y = static_cast<double>(crd.lattitude) / DegreeAccuracy;                // Перевод широты в градусы
        res.y = static_cast<int32_t>(cm_on_grad_y * grad_y);                                // Результат: широта, см

        double lat_rad = DegreeToRadian(grad_y);                                            // Широта в радианах
        double cm_on_grad_x = (EquatorLength / DegreesInFullCircle) * std::cos(lat_rad);    // Расстояние в одном градусе долготы, см
        double grad_x = static_cast<double>(crd.longitude) / DegreeAccuracy;                // Перевод долготы в градусы
        res.x = static_cast<int32_t>(cm_on_grad_x * grad_x);                                // Результат: долгота, см

        return res;
    }

    Cordinates PointToCordinate(const int32_t &lttd, const Point &pnt)
    {
        Cordinates res;

        double round_len_y = EquatorLength;                                                 // Длина окружности по широте, см
        double part_y = pnt.y / round_len_y;                                                // Определение части окружности по широте
        double grad_y = part_y * DegreesInFullCircle;                                       // Широта, градусы
        res.lattitude = static_cast<int32_t>(grad_y * DegreeAccuracy);

        double part_lttd = lttd / round_len_y;                                              // Определение части окружности по широте
        double grad_lttd = part_lttd * DegreesInFullCircle;                                 // Широта, градусы
        int32_t lattitude = static_cast<int32_t>(grad_lttd * DegreeAccuracy);
        double lat_rad = DegreeToRadian(static_cast<double>(lattitude) / DegreeAccuracy);   // Широта в радианах
        double round_len_x = round_len_y * std::cos(lat_rad);                               // Длина окружности по долготе
        double part_x = pnt.x / round_len_x;                                                // Определение части окружности по долготе
        double grad_x = part_x * DegreesInFullCircle;                                       // Долгота, градусы
        res.longitude = static_cast<int32_t>(grad_x * DegreeAccuracy);

        return res;
    }

    double CountTimeChangeParam(double val1, double val2, uint64_t ms)
    {
        return (val2 - val1) * 1000 / ms;
    }

    uint16_t MillisecondsToFall(int32_t alt, float zsp, float acc)
    {
        if (acc == 0)
            acc = GravityAcceleration;
        float d = (zsp * zsp) + (2 * acc * alt);
        return 1000 * (std::sqrt(d) - zsp) / acc;
    }

    Point FindDelta(float xsp, float ysp, float xacc, float yacc, uint16_t ms)
    {
        float ts = static_cast<float>(ms) / 1000.0;
        int32_t dx = (xacc / 2) * ts * ts + xsp * ts;
        int32_t dy = (yacc / 2) * ts * ts + ysp * ts;
        return {dx, dy};
    }

    Point FindNewPosition(int32_t x, int32_t y, int32_t z, float xsp, float ysp, float zsp, float xacc, float yacc, float zacc)
    {
        uint16_t t = MillisecondsToFall(z, zsp, zacc);
        Point p = FindDelta(xsp, ysp, xacc, yacc, t);
        return {x + p.x, y + p.y};
    }
}
