#ifndef BLIB_REFLECTION_PROVIDEDTYPES_HPP
#define BLIB_REFLECTION_PROVIDEDTYPES_HPP

#include <BLIB/Reflection/ReflectedObject.hpp>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <glm/glm.hpp>
#include <utility>

namespace bl
{
namespace refl
{
template<typename U, typename V>
struct ReflectedObject<std::pair<U, V>> {
    inline static const auto spec = makeSpec<std::pair<U, V>>(
        "stdPair", memberList(defineMember(1, "first", &std::pair<U, V>::first),
                              defineMember(2, "second", &std::pair<U, V>::second)));
};

template<typename U>
struct ReflectedObject<sf::Vector2<U>> {
    inline static const auto spec =
        makeSpec<sf::Vector2<U>>("sfVector2", memberList(defineMember(1, "x", &sf::Vector2<U>::x),
                                                         defineMember(2, "y", &sf::Vector2<U>::y)));
};

template<typename U>
struct ReflectedObject<sf::Vector3<U>> {
    inline static const auto spec =
        makeSpec<sf::Vector3<U>>("sfVector3", memberList(defineMember(1, "x", &sf::Vector3<U>::x),
                                                         defineMember(2, "y", &sf::Vector3<U>::y),
                                                         defineMember(3, "z", &sf::Vector3<U>::z)));
};

template<typename U>
struct ReflectedObject<sf::Rect<U>> {
    inline static const auto spec = makeSpec<sf::Rect<U>>(
        "sfRect", memberList(defineMember(1, "position", &sf::Rect<U>::position),
                             defineMember(2, "size", &sf::Rect<U>::size)));
};

template<typename U, glm::qualifier P>
struct ReflectedObject<glm::vec<2, U, P>> {
    inline static const auto spec = makeSpec<glm::vec<2, U, P>>(
        "glmVec2", memberList(defineMember(1, "x", &glm::vec<2, U, P>::x),
                              defineMember(2, "y", &glm::vec<2, U, P>::y)));
};

template<typename U, glm::qualifier P>
struct ReflectedObject<glm::vec<3, U, P>> {
    inline static const auto spec = makeSpec<glm::vec<3, U, P>>(
        "glmVec3", memberList(defineMember(1, "x", &glm::vec<3, U, P>::x),
                              defineMember(2, "y", &glm::vec<3, U, P>::y),
                              defineMember(3, "z", &glm::vec<3, U, P>::z)));
};

template<typename U, glm::qualifier P>
struct ReflectedObject<glm::vec<4, U, P>> {
    inline static const auto spec = makeSpec<glm::vec<4, U, P>>(
        "glmVec4", memberList(defineMember(1, "x", &glm::vec<4, U, P>::x),
                              defineMember(2, "y", &glm::vec<4, U, P>::y),
                              defineMember(3, "z", &glm::vec<4, U, P>::z),
                              defineMember(4, "w", &glm::vec<4, U, P>::w)));
};

template<typename U, glm::qualifier P>
struct ReflectedObject<glm::qua<U, P>> {
    inline static const auto spec =
        makeSpec<glm::qua<U, P>>("glmQuat", memberList(defineMember(1, "x", &glm::qua<U, P>::x),
                                                       defineMember(2, "y", &glm::qua<U, P>::y),
                                                       defineMember(3, "z", &glm::qua<U, P>::z),
                                                       defineMember(4, "w", &glm::qua<U, P>::w)));
};

} // namespace refl
} // namespace bl

#endif
