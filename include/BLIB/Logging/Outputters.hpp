#ifndef BLIB_LOGGING_OUTPUTTERS_HPP
#define BLIB_LOGGING_OUTPUTTERS_HPP

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/String.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <glm/glm.hpp>
#include <ostream>
#include <type_traits>

template<typename E>
typename std::enable_if<std::is_enum<E>::value, std::ostream&>::type operator<<(std::ostream& os,
                                                                                E v) {
    using T = std::underlying_type_t<E>;
    if constexpr (std::is_same<T, std::uint8_t>::value) { os << static_cast<unsigned int>(v); }
    else if constexpr (std::is_same<T, std::int8_t>::value) { os << static_cast<int>(v); }
    else { os << static_cast<T>(v); }
    return os;
}

namespace sf
{
template<typename T>
std::ostream& operator<<(std::ostream& os, const sf::Vector2<T>& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const sf::Vector3<T>& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const sf::Rect<T>& r) {
    os << "(" << r.position.x << ", " << r.position.y << ", " << r.size.x << ", " << r.size.y
       << ")";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const sf::String& str) {
    os << str.toAnsiString();
    return os;
}

} // namespace sf

template<typename T, glm::qualifier Q>
std::ostream& operator<<(std::ostream& os, const glm::vec<1, T, Q>& v) {
    os << "(" << v.x << ")";
    return os;
}

template<typename T, glm::qualifier Q>
std::ostream& operator<<(std::ostream& os, const glm::vec<2, T, Q>& v) {
    os << "(" << v.x << ", " << v.y << ")";
    return os;
}

template<typename T, glm::qualifier Q>
std::ostream& operator<<(std::ostream& os, const glm::vec<3, T, Q>& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

template<typename T, glm::qualifier Q>
std::ostream& operator<<(std::ostream& os, const glm::vec<4, T, Q>& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    return os;
}

#endif
