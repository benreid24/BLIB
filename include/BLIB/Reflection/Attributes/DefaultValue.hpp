#ifndef BLIB_REFLECTION_ATTRIBUTES_DEFAULTVALUE_HPP
#define BLIB_REFLECTION_ATTRIBUTES_DEFAULTVALUE_HPP

#include <utility>

namespace bl
{
namespace refl
{
/// Collection of core built-in attributes for reflection
namespace attr
{
/**
 * @brief Built-in attribute for providing a default value for reflected members
 *
 * @tparam T The type of member the default is for
 * @ingroup Reflection
 */
template<typename T>
struct DefaultValue {
    DefaultValue(T&& value)
    : value(std::forward<T>(value)) {}

    T value;
};

/**
 * @brief Deduction helper to create a DefaultValue attribute
 *
 * @tparam T The type of member the default is for
 * @param value The default value to use
 * @return The attribute
 * @ingroup Reflection
 */
template<typename T>
DefaultValue<T> withDefaultValue(T&& value) {
    return DefaultValue<T>(std::forward<T>(value));
}

} // namespace attr
} // namespace refl
} // namespace bl

#endif
