#ifndef BLIB_ECS_TAGS_HPP
#define BLIB_ECS_TAGS_HPP

#include <BLIB/ECS/ComponentMask.hpp>
#include <type_traits>

namespace bl
{
namespace ecs
{
class Registry;

template<typename TRequire, typename TOptional, typename TExclude>
class View;

template<typename TRequire, typename TOptional>
class ComponentSet;

namespace tx
{
template<typename... Ts>
struct TransactionComponentRead;

template<typename... Ts>
struct TransactionComponentWrite;
} // namespace tx

/**
 * @brief Tag that indicates a set of components are required
 *
 * @tparam ...TComs The set of required components
 * @ingroup ECS
 */
template<typename... TComs>
struct Require {};

/**
 * @brief Tag that indicates a set of components are optional
 *
 * @tparam ...TComs The set of optional components
 * @ingroup ECS
 */
template<typename... TComs>
struct Optional {};

/**
 * @brief Tag that indicates a set of components are excluded
 *
 * @tparam ...TComs The set of excluded components
 * @ingroup ECS
 */
template<typename... TComs>
struct Exclude {};

template<typename TRequire, typename TOptional = Optional<>, typename TExclude = Exclude<>>
struct Tags {};

/**
 * @brief Combined set of tags describing an archetype of components
 *
 * @tparam ...TReqComs Required components
 * @tparam ...TOptComs Optional components
 * @tparam ...TExcComs Excluded components
 * @ingroup ECS
 */
template<typename... TReqComs, typename... TOptComs, typename... TExcComs>
struct Tags<Require<TReqComs...>, Optional<TOptComs...>, Exclude<TExcComs...>> {
    /// The corresponding ComponentSet type for this set of components
    using TComponentSet = ComponentSet<Require<TReqComs...>, Optional<TOptComs...>>;

    /// The corresponding view type for this set of components
    using TView = View<Require<TReqComs...>, Optional<TOptComs...>, Exclude<TExcComs...>>;

    /// The number of required and optional components
    static constexpr std::size_t NumComponents = sizeof...(TReqComs) + sizeof...(TOptComs);

    /// The read transaction type for the required and optional components
    using ReadTx = tx::TransactionComponentRead<TReqComs..., TOptComs...>;

    /// The write transaction type for the required and optional components
    using WriteTx = tx::TransactionComponentWrite<TReqComs..., TOptComs...>;

    /**
     * Helper to construct the component mask describing the tagged set of components
     */
    static ComponentMask createMask(Registry& registry);

    /**
     * @brief Helper to get or create a view for this tagged set of components
     *
     * @param registry The registry to get the view from
     * @return The view for this set of components
     */
    static TView* getView(Registry& registry);

private:
    template<typename... Ts>
    static ComponentMask::SimpleMask createHelper(Registry& registry);
};

} // namespace ecs
} // namespace bl

#endif
