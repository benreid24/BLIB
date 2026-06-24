#ifndef BLIB_REFLECTION_REFLECTEDMEMBER_HPP
#define BLIB_REFLECTION_REFLECTEDMEMBER_HPP

#include <BLIB/Reflection/AttributeList.hpp>
#include <cstdint>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <typeindex>

namespace bl
{
/// Object reflection and descriptor system
namespace refl
{
/**
 * @brief Non-template base class for reflected members to provide easy access to common info
 *
 * @ingroup Reflection
 */
class ReflectedMemberBase {
public:
    /**
     * @brief Creates the member base
     *
     * @param id The id of the member
     * @param name The name of the member
     * @param type The type of the member
     */
    ReflectedMemberBase(std::uint16_t id, std::string_view name, std::type_index type);

    /**
     * @brief Returns the id of the member
     */
    std::uint16_t getId() const { return id; }

    /**
     * @brief Returns the name of the member
     */
    std::string_view getName() const { return name; }

    /**
     * @brief Returns the type of the member
     */
    std::type_index getType() const { return type; }

private:
    std::uint16_t id;
    std::string_view name;
    std::type_index type;
};

/**
 * @brief Class for reflected members of reflected objects
 *
 * @tparam ObjectType The owning object type of the member
 * @tparam MemberType The type of the member itself
 * @tparam ...Attributes The types of the attributes to store on this member
 * @ingroup Reflection
 */
template<typename ObjectType, typename MemberType, typename... Attributes>
class ReflectedMember : public ReflectedMemberBase {
public:
    /**
     * @brief Creates the reflected member
     *
     * @param id The id of the member
     * @param name The name of the member
     * @param memberPointer The pointer to the member in the object
     * @param ...attrs The attributes to store on this member
     */
    ReflectedMember(std::uint16_t id, std::string_view name, MemberType ObjectType::* memberPointer,
                    Attributes&&... attrs)
    : ReflectedMemberBase(id, name, typeid(MemberType))
    , memberPointer(memberPointer)
    , attributes(std::forward<Attributes>(attrs)...) {}

    /**
     * @brief Helper to access the member of an object
     *
     * @param object The object to get the member from
     * @return A reference to the member on the object
     */
    MemberType& getMember(ObjectType& object) const { return object.*memberPointer; }

    /**
     * @brief Helper to access the member of an object
     *
     * @param object The object to get the member from
     * @return A reference to the member on the object
     */
    const MemberType& getMember(const ObjectType& object) const { return object.*memberPointer; }

    /**
     * @brief Returns the attribute list for this member
     */
    const AttributeList<Attributes...>& getAttributes() const { return attributes; }

    /**
     * @brief Test whether an attribute is present
     *
     * @tparam T The type of attribute to test for
     * @return True if the attribute is present, false otherwise
     */
    template<typename T>
    static constexpr bool hasAttribute() {
        return detail::HasAttributeRaw<T, Attributes...>;
    }

    /**
     * @brief Returns a pointer to the given attribute type, or nullptr if not present
     *
     * @tparam T The type of attribute to get
     * @return A pointer to the attribute, or nullptr if not present
     */
    template<typename T>
    const T* getAttribute() const {
        return detail::getAttribute<T>(attributes);
    }

private:
    MemberType ObjectType::* memberPointer;
    AttributeList<Attributes...> attributes;
};

/**
 * @brief Deduction helper to create a ReflectedMember object
 *
 * @tparam ObjectType The owning object type of the member
 * @tparam MemberType The type of the member itself
 * @tparam ...Attributes The types of the attributes to store on this member
 * @param id The id of the member
 * @param name The name of the member
 * @param memberPointer The pointer to the member in the object
 * @param ...attrs The attributes to store on this member
 * @return A ReflectedMember object for the given member
 * @ingroup Reflection
 */
template<typename ObjectType, typename MemberType, typename... Attributes>
ReflectedMember<ObjectType, MemberType, Attributes...> defineMember(
    std::uint16_t id, std::string_view name, MemberType ObjectType::* memberPointer,
    Attributes&&... attrs) {
    return ReflectedMember<ObjectType, MemberType, Attributes...>(
        id, name, memberPointer, std::forward<Attributes>(attrs)...);
}

/**
 * @brief Deduction helper to define the list of members for a reflected object
 *
 * @tparam ...Members The types of the members to store in the list
 * @param ...members The members to store in the list
 * @return The list of members for the reflected object
 * @ingroup Reflection
 */
template<typename... Members>
auto memberList(Members&&... members) {
    return std::make_tuple(std::forward<Members>(members)...);
}

} // namespace refl
} // namespace bl

#endif
