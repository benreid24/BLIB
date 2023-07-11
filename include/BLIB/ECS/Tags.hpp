#ifndef BLIB_ECS_TAGS_HPP
#define BLIB_ECS_TAGS_HPP

#include <BLIB/ECS/ComponentMask.hpp>
#include <BLIB/ECS/ComponentSet.hpp>
#include <type_traits>

namespace bl
{
namespace ecs
{
class Registry;

template<typename... TComs>
struct Require {};

template<typename... TComs>
struct Optional {};

template<typename... TComs>
struct Exclude {};

template<typename TRequire, typename TOptional = Optional<>, typename TExclude = Exclude<>>
struct Tags {};

} // namespace ecs
} // namespace bl

#endif
