#ifndef BLIB_COMPONENTS_TOGGLER_HPP
#define BLIB_COMPONENTS_TOGGLER_HPP

namespace bl
{
namespace com
{
/**
 * @brief Basic component to toggle a boolean value over time
 *
 * @ingroup Components
 */
struct Toggler {
    /**
     * @brief Creates the toggler component
     *
     * @param onPeriod The amount of time to keep the value true
     * @param offPeriod The amount of time to keep the value false
     * @param value Pointer to the value to toggle
     */
    Toggler(float onPeriod, float offPeriod, bool* value)
    : onPeriod(onPeriod)
    , offPeriod(offPeriod)
    , value(value)
    , time(0.f) {}

    float onPeriod;
    float offPeriod;
    bool* value;
    float time;
};

} // namespace com
} // namespace bl

#endif
