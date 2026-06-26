#ifndef BLIB_REFLECTION_DETAIL_ISSPEC_HPP
#define BLIB_REFLECTION_DETAIL_ISSPEC_HPP

#include <BLIB/Reflection/Spec.hpp>
#include <type_traits>

namespace bl
{
namespace refl
{
namespace detail
{
template<typename T>
struct IsSpec : std::false_type {};

template<typename T, typename M, typename... A>
struct IsSpec<Spec<T, M, A...>> : std::true_type {};

} // namespace detail
} // namespace refl
} // namespace bl

#endif
