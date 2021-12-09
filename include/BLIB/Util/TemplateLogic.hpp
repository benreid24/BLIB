#ifndef BLIB_UTIL_TEMPLATELOGIC_HPP
#define BLIB_UTIL_TEMPLATELOGIC_HPP

#include <type_traits>

namespace bl
{
namespace util
{
/**
 * @brief Does the boolean negation of true_type and false_type
 *
 * @tparam T Value to negate
 */
template<typename T>
struct Not {};

template<>
struct Not<std::true_type> {
    using value = std::false_type;
};

template<>
struct Not<std::false_type> {
    using value = std::true_type;
};

template<typename T>
using NotV = Not<T>::value;

} // namespace util
} // namespace bl

#endif
