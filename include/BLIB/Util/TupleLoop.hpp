#ifndef BLIB_UTIL_TUPLELOOP_HPP
#define BLIB_UTIL_TUPLELOOP_HPP

#include <tuple>
#include <utility>

namespace bl
{
namespace util
{
/**
 * @brief Base case for TupleLoop utility
 *
 * @tparam I Last index
 * @tparam FuncT callback method type
 * @tparam Tp Tuple types
 * @return std::enable_if<I == sizeof...(Tp), void>::type False type
 *
 * @ingroup Util
 */
template<std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type TupleLoop(std::tuple<Tp...>&,
                                                                         FuncT) {}

/**
 * @brief Helper utility to iterate over a tuple
 *
 * @param t Tuple to iterate over
 * @param f Callback to call for each tuple. Takes an index and a tuple type
 * @tparam I Starting index
 * @tparam FuncT Callback method type signature
 * @tparam Tp Tuple types
 *
 * @ingroup Util
 */
template<std::size_t I = 0, typename FuncT, typename... Tp>
    inline typename std::enable_if <
    I<sizeof...(Tp), void>::type TupleLoop(std::tuple<Tp...>& t, FuncT f) {
    f(I, std::get<I>(t));
    TupleLoop<I + 1, FuncT, Tp...>(t, f);
}

} // namespace util
} // namespace bl

#endif