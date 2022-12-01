#ifndef CUSTOMOBJECT_HPP
#define CUSTOMOBJECT_HPP

#include <BLIB/Serialization.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>

struct Character {
    std::string name;
    sf::Vector2i position;
    unsigned int hp;

    bool loadFromJsonFile(const std::string& path);
    bool loadFromJson(std::istream& stream);
    bool saveToBinary(std::ostream& output) const;
    bool loadFromBinary(std::istream& stream);
};

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<Character> : public SerializableObjectBase {
    SerializableField<1, Character, std::string> name;
    SerializableField<2, Character, sf::Vector2i> position;
    SerializableField<3, Character, unsigned int> hp;

    SerializableObject()
    : name("name", *this, &Character::name, SerializableFieldBase::Required{})
    , position("position", *this, &Character::position, SerializableFieldBase::Required{})
    , hp("hp", *this, &Character::hp, SerializableFieldBase::Required{}) {}
};
} // namespace serial
} // namespace bl

#endif
