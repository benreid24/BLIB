#ifndef BLIB_REFLECTION_ISMEMBER_HPP
#define BLIB_REFLECTION_ISMEMBER_HPP

#include <BLIB/Reflection/ReflectedMember.hpp>
#include <type_traits>

namespace bl
{
namespace refl
{
/// Implementation details only
namespace detail
{
template<typename T>
struct IsMember : std::false_type {};

template<typename C, typename M, typename... Attrs>
struct IsMember<ReflectedMember<C, M, Attrs...>> : std::true_type {};

template<typename T>
using IsMemberT = typename IsMember<T>::type;

} // namespace detail
} // namespace refl
} // namespace bl

#endif
