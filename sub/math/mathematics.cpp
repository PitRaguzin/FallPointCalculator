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

    Point WGS84ToProjection(const GeographicPoint &crd)
    {
        Point res;

        double cm_on_grad_y = EquatorLength / DegreesInFullCircle;                          // Расстояние в одном градусе широты, см
        double grad_y = static_cast<double>(crd.lattitude) / DegreeAccuracy;                // Перевод широты в градусы
        res.y = static_cast<int32_t>(cm_on_grad_y * grad_y);                                // Результат: широта, см

        double lat_rad = DegreeToRadian(grad_y);                                            // Широта в радианах
        double cm_on_grad_x = (EquatorLength / DegreesInFullCircle) * std::cos(lat_rad);    // Расстояние в одном градусе долготы, см
        double grad_x = static_cast<double>(crd.longitude) / DegreeAccuracy;                // Перевод долготы в градусы
        res.x = static_cast<int32_t>(cm_on_grad_x * grad_x);                                // Результат: долгота, см

        res.z = crd.altitude;                                                               // Результат: высота, см

        return res;
    }

    GeographicPoint ProjectionToWGS84(const int32_t &lttd, const Point &pnt)
    {
        GeographicPoint res;

        double round_len_y = EquatorLength;                                                 // Длина окружности по широте, см
        double part_y = pnt.y / round_len_y;                                                // Определение части окружности по широте
        double grad_y = part_y * DegreesInFullCircle;                                       // Широта, градусы
        res.lattitude = static_cast<int32_t>(grad_y * DegreeAccuracy);                      // Результат: широта, град * 1e7

        double part_lttd = lttd / round_len_y;                                              // Определение части окружности по широте
        double grad_lttd = part_lttd * DegreesInFullCircle;                                 // Широта, градусы
        int32_t lattitude = static_cast<int32_t>(grad_lttd * DegreeAccuracy);
        double lat_rad = DegreeToRadian(static_cast<double>(lattitude) / DegreeAccuracy);   // Широта в радианах
        double round_len_x = round_len_y * std::cos(lat_rad);                               // Длина окружности по долготе
        double part_x = pnt.x / round_len_x;                                                // Определение части окружности по долготе
        double grad_x = part_x * DegreesInFullCircle;                                       // Долгота, градусы
        res.longitude = static_cast<int32_t>(grad_x * DegreeAccuracy);                      // Результат: долгота, град * 1e7

        res.altitude = pnt.z;                                                               // Результат: высота, см

        return res;
    }

    Point WGS84ToXYZ(const GeographicPoint &gp)
    {
        double lat = static_cast<double>(gp.lattitude) / DegreeAccuracy;
        double lon = static_cast<double>(gp.longitude) / DegreeAccuracy;
        double alt = static_cast<double>(gp.altitude);
        double sinlat = std::sin(DegreeToRadian(lat)); // Синус широты
        double sinlon = std::sin(DegreeToRadian(lon)); // Синус долготы
        double coslat = std::cos(DegreeToRadian(lat)); // Косинус широты
        double coslon = std::cos(DegreeToRadian(lon)); // Косинус долготы
        double Wsqw = 1 - esqw * sinlat * sinlat; // Квадрат первой основной сфероидной функции
        double N = a / std::sqrt(Wsqw); // Радиус кривизны первого вертикала
        return
            {
                static_cast<int32_t>((N + alt) * coslat * coslon),
                static_cast<int32_t>((N + alt) * coslat * sinlon),
                static_cast<int32_t>((N * (1 - esqw) + alt) * sinlat)
            };
    }

    GeographicPoint XYZToWGS84(const Point &p)
    {
        double x = static_cast<double>(p.x);
        double y = static_cast<double>(p.y);
        double z = static_cast<double>(p.z);
        double Q = std::sqrt(x * x + y * y);
        double mu = std::atan(z * a / (Q * b));
        GeographicPoint gp;
        double latrad = std::atan((z + e2sqw * b * std::pow(std::sin(mu), 3)) / (Q - esqw * a * std::pow(std::cos(mu), 3)));
        gp.lattitude = static_cast<int32_t>(RadianToDegree(latrad * DegreeAccuracy));
        gp.longitude = static_cast<int32_t>(RadianToDegree(std::atan(y / x)) * DegreeAccuracy);
        gp.altitude = Q / std::cos(latrad) - a / std::sqrt(1 - esqw * std::sin(latrad) * std::sin(latrad)) - p.dz;
        return gp;
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

    Point FindDelta(float xsp, float ysp, float zsp, float xacc, float yacc, float zacc, uint16_t ms)
    {
        float ts = static_cast<float>(ms) / 1000.0;
        int32_t dx = (xacc / 2) * ts * ts + xsp * ts;
        int32_t dy = (yacc / 2) * ts * ts + ysp * ts;
        int32_t dz = (zacc / 2) * ts * ts + zsp * ts;
        return {dx, dy, dz};
    }

    Point FindNewPosition(int32_t x, int32_t y, int32_t z, int32_t h, float xsp, float ysp, float zsp, float hsp, float xacc, float yacc, float zacc, float hacc)
    {
        uint16_t t = MillisecondsToFall(h, hsp, hacc);
        Point p = FindDelta(xsp, ysp, zsp, xacc, yacc, zacc, t);
        return {x + p.x, y + p.y, z + p.z};
    }
}
