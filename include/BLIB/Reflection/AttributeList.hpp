#ifndef BLIB_REFLECTION_ATTRIBUTE_LIST_HPP
#define BLIB_REFLECTION_ATTRIBUTE_LIST_HPP

#include <tuple>
#include <type_traits>

namespace bl
{
namespace refl
{
/**
 * @brief Helper type for storing arbitrary attributes on reflected types
 * @tparam ...TAttrs The types of attributes to store
 * @ingroup Reflection
 */
template<typename... TAttrs>
struct AttributeList {
    std::tuple<TAttrs...> attrs;
};

namespace detail
{
template<typename T, typename... TAttrs>
constexpr bool HasAttributeRaw = (std::is_same_v<T, TAttrs> || ...);

template<typename T, typename AttributeList>
struct HasAttribute {
    static constexpr bool value = false;
};

template<typename T, typename... TAttrs>
struct HasAttribute<T, AttributeList<TAttrs...>> {
    static constexpr bool value = HasAttributeRaw<T, TAttrs...>;
};

template<typename T, typename... TAttrs>
const T* getAttribute(const AttributeList<TAttrs...>& attributes) {
    if constexpr (HasAttributeRaw<T, TAttrs...>) { return &std::get<T>(attributes.attrs); }
    else { return nullptr; }
}

} // namespace detail

} // namespace refl
} // namespace bl

#endif
