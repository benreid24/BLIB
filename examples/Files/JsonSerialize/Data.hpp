#ifndef EXAMPLE_DATA_HPP
#define EXAMPLE_DATA_HPP

#include <BLIB/Files/JSON.hpp>

class Map;

namespace bl
{
namespace file
{
namespace json
{
template<>
struct Serializer<Map> {
    static bool deserialize(Map& result, const Value& value);

    static Value serialize(const Map& value);
};
} // namespace json
} // namespace file
} // namespace bl

/**
 * @brief Npc inherits from SerializableObject so it can be directly serialized and can also be a
 *        SerializableField in parent objects
 *
 */
struct Npc : public bl::file::json::SerializableObject {
    Npc()
    : name("name", *this)
    , health("health", *this) {}

    // Copy constructor is required for nested SerializableObjects
    Npc(const Npc& copy)
    : Npc() {
        name.setValue(copy.name.getValue());
        health.setValue(copy.health.getValue());
    }

    Npc(const std::string& n, int h)
    : Npc() {
        name.setValue(n);
        health.setValue(h);
    }

    bl::file::json::SerializableField<std::string> name;
    bl::file::json::SerializableField<int> health;
};

/**
 * @brief Map does not inherit SerializableObject and instead has a specialization of the Serializer
 *        struct implemented for it
 *
 */
struct Map {
    std::string name;
    std::string data;
};

/**
 * @brief World inherits from SerializableObject and can be directly serialized and deserialized
 *
 */
struct World : public bl::file::json::SerializableObject {
    bl::file::json::SerializableField<Map> map;
    bl::file::json::SerializableField<std::string> test;
    bl::file::json::SerializableField<std::vector<Npc>> npcs;

    World()
    : map("map", *this)
    , test("test", *this)
    , npcs("npcs", *this) {
        test.setValue("something");
    }

    World(const Map& m, const std::vector<Npc>& n)
    : World() {
        map.setValue(m);
        npcs.setValue(n);
    }
};

#endif
