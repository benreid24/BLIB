#ifndef BLIB_ECS_COMPONENTMASKIMPL_HPP
#define BLIB_ECS_COMPONENTMASKIMPL_HPP

#ifndef BLIB_ECS_REGISTRY_HPP
#error "Internal file included out of order"
#endif

#include <BLIB/ECS/ComponentMask.hpp>
#include <BLIB/ECS/Tags.hpp>

namespace bl
{
namespace ecs
{
template<typename... TReqComs, typename... TOptComs, typename... TExcComs>
struct Tags<Require<TReqComs...>, Optional<TOptComs...>, Exclude<TExcComs...>> {
    using TComponentSet = ComponentSet<Require<TReqComs...>, Optional<TOptComs...>>;
    static constexpr std::size_t NumComponents = sizeof...(TReqComs) + sizeof...(TOptComs);

    static ComponentMask createMask(Registry& registry) {
        ComponentMask result;
        result.required = createHelper<TReqComs...>(registry);
        result.excluded = createHelper<TExcComs...>(registry);
        result.optional = createHelper<TOptComs...>(registry);
        return result;
    }

private:
    template<typename... Ts>
    static ComponentMask::SimpleMask createHelper(Registry& registry) {
        if constexpr (sizeof...(Ts) > 0) {
            constexpr ComponentMask::SimpleMask One = 0x1;
            return ((One << registry.getAllComponents<Ts>().ComponentIndex) | ...);
        }
        else { return ComponentMask::EmptyMask; }
    }
};

} // namespace ecs
} // namespace bl

#endif
