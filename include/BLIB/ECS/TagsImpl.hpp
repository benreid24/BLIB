#ifndef BLIB_ECS_COMPONENTMASKIMPL_HPP
#define BLIB_ECS_COMPONENTMASKIMPL_HPP

#ifndef BLIB_ECS_REGISTRY_HPP
#error "Internal file included out of order"
#endif

#include <BLIB/ECS/ComponentMask.hpp>
#include <BLIB/ECS/Tags.hpp>
#include <BLIB/ECS/View.hpp>

namespace bl
{
namespace ecs
{
template<typename... TReqComs, typename... TOptComs, typename... TExcComs>
ComponentMask Tags<Require<TReqComs...>, Optional<TOptComs...>, Exclude<TExcComs...>>::createMask(
    Registry& registry) {
    ComponentMask result;
    result.required = createHelper<TReqComs...>(registry);
    result.excluded = createHelper<TExcComs...>(registry);
    result.optional = createHelper<TOptComs...>(registry);
    return result;
}

template<typename... TReqComs, typename... TOptComs, typename... TExcComs>
typename Tags<Require<TReqComs...>, Optional<TOptComs...>, Exclude<TExcComs...>>::TView*
Tags<Require<TReqComs...>, Optional<TOptComs...>, Exclude<TExcComs...>>::getView(
    Registry& registry) {
    return registry
        .getOrCreateView<Require<TReqComs...>, Optional<TOptComs...>, Exclude<TExcComs...>>();
}

template<typename... TReqComs, typename... TOptComs, typename... TExcComs>
template<typename... Ts>
ComponentMask::SimpleMask Tags<Require<TReqComs...>, Optional<TOptComs...>,
                               Exclude<TExcComs...>>::createHelper(Registry& registry) {
    if constexpr (sizeof...(Ts) > 0) {
        constexpr ComponentMask::SimpleMask One = 0x1;
        return ((One << registry.getAllComponents<Ts>().ComponentIndex) | ...);
    }
    else { return ComponentMask::EmptyMask; }
}

} // namespace ecs
} // namespace bl

#endif
