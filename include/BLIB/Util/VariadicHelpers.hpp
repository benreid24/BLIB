#ifndef BLIB_UTIL_VARIADICHELPERS_HPP
#define BLIB_UTIL_VARIADICHELPERS_HPP

#include <type_traits>

namespace bl
{
namespace util
{
/**
 * @brief Helper that tests whether a list of types contains a given type
 *
 * @tparam T The type to search for
 * @tparam ...List The list of types to search in
 * @ingroup Util
 */
template<typename T, typename... List>
struct VariadicListContains : public std::false_type {};

template<typename T, typename First, typename... List>
struct VariadicListContains<T, First, List...>
: public std::conditional_t<std::is_same_v<T, First>, std::true_type,
                            VariadicListContains<T, List...>> {};

/**
 * @brief Helper to capture a list of types into a single type
 *
 * @tparam ...Ts The list of types to capture
 * @ingroup Util
 */
template<typename... Ts>
struct Variadic {};

/**
 * @brief Helper to test whether one list of types is fully contained by another
 *
 * @tparam Outer The outer list to test if it contains the inner list
 * @tparam Inner The list to test to see if fully contained by the outer list
 * @ingroup Util
 */
template<typename Inner, typename Outer>
struct VariadicSetsContained : public std::false_type {};

template<typename... Outers>
struct VariadicSetsContained<Variadic<>, Variadic<Outers...>> : public std::true_type {};

template<typename First, typename... Rest, typename... Outers>
struct VariadicSetsContained<Variadic<First, Rest...>, Variadic<Outers...>>
: public std::conditional_t<VariadicListContains<First, Outers...>::value,
                            VariadicSetsContained<Variadic<Rest...>, Variadic<Outers...>>,
                            std::false_type> {};

} // namespace util
} // namespace bl

#endif
