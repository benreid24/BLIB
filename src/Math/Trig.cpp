#include <BLIB/Math/Trig.hpp>

#include <cmath>

namespace bl
{
namespace math
{
namespace
{
float cachedCos[1440];
bool cosAvail = false;
float cachedSin[1440];
bool sinAvail = false;

void cacheCos() {
    for (unsigned int i = 0; i < 1440; ++i) {
        cachedCos[i] = std::cos(degreesToRadians(static_cast<float>(i) / 4.f));
    }
    cosAvail = true;
}

void cacheSin() {
    for (unsigned int i = 0; i < 1440; ++i) {
        cachedSin[i] = std::sin(degreesToRadians(static_cast<float>(i) / 4.f));
    }
    sinAvail = true;
}
} // namespace

float cos(float a) {
    if (!cosAvail) { cacheCos(); }
    if (a < 0.f) { a += std::ceil(-a / 360.f) * 360.f; }
    return cachedCos[static_cast<unsigned int>(a * 4.f) % 1440];
}

float sin(float a) {
    if (!sinAvail) { cacheSin(); }
    if (a < 0.f) { a += std::ceil(-a / 360.f) * 360.f; }
    return cachedSin[static_cast<unsigned int>(a * 4.f) % 1440];
}

} // namespace math
} // namespace bl
