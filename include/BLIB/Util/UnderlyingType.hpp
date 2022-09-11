#ifndef BLIB_UTIL_UNDERLYINGTYPE_HPP
#define BLIB_UTIL_UNDERLYINGTYPE_HPP

#include <type_traits>

namespace bl
{
namespace util
{
/**
 * @brief Provides the underlying type of an enum with the added benefit of allow integral types to
 *        pass-through instead of throwing errors.
 *
 * @tparam T The type to get the underlying type for
 * @ingroup Util
 */
template<typename T, bool = std::is_enum_v<T>>
struct UnderlyingType {};

template<typename U>
struct UnderlyingType<U, false> {
    using type = U;
};

template<typename U>
struct UnderlyingType<U, true> {
    using type = std::underlying_type_t<U>;
};

template<typename T>
using UnderlyingTypeT = typename UnderlyingType<T>::type;

} // namespace util
} // namespace bl

#endif
