#include <BLIB/Render/Settings.hpp>

namespace bl
{
namespace rc
{
namespace
{
constexpr float DefaultGamma = 2.2f;
}

Settings::Settings()
: gamma(DefaultGamma)
, dirty(true) {}

Settings& Settings::setGamma(float g) {
    gamma = g;
    dirty = true;
    return *this;
}

} // namespace rc
} // namespace bl
