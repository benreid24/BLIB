#ifndef MAP_HPP
#define MAP_HPP

#include "Character.hpp"
#include <BLIB/Serialization.hpp>
#include <vector>

struct Map {
    static const std::string NpcPath;

    std::string name;
    sf::Vector2i size;
    std::vector<std::string> npcs;

    bool loadFromJsonFile(const std::string& path);
    bool saveToBinary(std::ostream& output) const;
};

namespace bl
{
namespace serial
{
template<>
struct SerializableObject<Map> : public SerializableObjectBase {
    SerializableField<1, Map, std::string> name;
    SerializableField<2, Map, sf::Vector2i> size;
    SerializableField<3, Map, std::vector<std::string>> npcs;

    SerializableObject()
    : name("name", *this, &Map::name, SerializableFieldBase::Required{})
    , size("size", *this, &Map::size, SerializableFieldBase::Required{})
    , npcs("npcs", *this, &Map::npcs, SerializableFieldBase::Required{}) {}
};
} // namespace serial
} // namespace bl

#endif
