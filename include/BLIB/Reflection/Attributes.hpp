#ifndef BLIB_REFLECTION_ATTRIBUTES_HPP
#define BLIB_REFLECTION_ATTRIBUTES_HPP

#include <BLIB/Reflection/ReflectedObject.hpp>

namespace bl
{
namespace refl
{
/**
 * @brief Interface for getting attributes of reflected objects
 *
 * @ingroup Reflection
 */
struct Attributes {
    /**
     * @brief Tests whether an attribute is present for a reflected object
     *
     * @tparam ObjectType The type of the reflected object
     * @tparam Attribute The type of the attribute to test for
     * @return True if the attribute is present, false otherwise
     */
    template<typename ObjectType, typename Attribute>
    static constexpr bool hasAttribute() {
        using Reflected = ReflectedObject<ObjectType>;
        using TSpec     = std::decay_t<decltype(Reflected::spec)>;
        static_assert(detail::IsSpec<TSpec>::value,
                      "ReflectedObject<T>::spec() must return a refl::Spec");
        return detail::HasAttribute<Attribute,
                                    std::decay_t<decltype(Reflected::spec.attributes)>>::value;
    }

    /**
     * @brief Returns the full attribute list for a reflected object
     *
     * @tparam ObjectType The type of the reflected object
     * @return The attribute list for the reflected object
     */
    template<typename ObjectType>
    static auto getAttributes() {
        using Reflected = ReflectedObject<ObjectType>;
        using TSpec     = std::decay_t<decltype(Reflected::spec)>;
        static_assert(detail::IsSpec<TSpec>::value,
                      "ReflectedObject<T>::spec() must return a refl::Spec");
        return Reflected::spec.attributes;
    }

    /**
     * @brief Returns a specific attribute for a reflected object
     *
     * @tparam ObjectType The type of the reflected object
     * @tparam Attribute The type of the attribute to get
     * @return The attribute if present, nullptr otherwise
     */
    template<typename ObjectType, typename Attribute>
    static const Attribute* getAttribute() {
        using Reflected = ReflectedObject<ObjectType>;
        using TSpec     = std::decay_t<decltype(Reflected::spec)>;
        static_assert(detail::IsSpec<TSpec>::value,
                      "ReflectedObject<T>::spec() must return a refl::Spec");
        return detail::getAttribute<Attribute>(Reflected::spec.attributes);
    }
};

} // namespace refl
} // namespace bl

#endif
