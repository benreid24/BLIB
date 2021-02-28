#include "Data.hpp"
#include <BLIB/Files/JSON.hpp>
#include <iostream>

namespace bl
{
namespace json
{
bool Serializer<Map>::deserialize(Map& result, const Value& value) {
    RGroup rg = value.getAsGroup();
    if (!rg.has_value()) return false;
    const Group& group = rg.value();

    String name = group.getString("name");
    if (!name.has_value()) return false;
    result.name = name.value();

    String data = group.getString("data");
    if (!data.has_value()) return false;
    result.data = data.value();

    return true;
}

Value Serializer<Map>::serialize(const Map& value) {
    Group group;
    group.addField("name", {value.name});
    group.addField("data", {value.data});
    return {group};
}

} // namespace json
} // namespace bl

int main() {
    // Create a world
    World world({"mapName", "mapData"}, {Npc("Guy", 100), Npc("Woman", 105), Npc("Dog", 50)});

    // Save our world
    bl::JSON::saveToFile("world.json", world.serialize());

    // Load it again
    World loaded;
    if (!loaded.deserialize(bl::JSON::loadFromFile("world.json"))) {
        std::cout << "Error loading world\n";
        return 1;
    }
    std::cout << "Loaded the world\n";
    return 0;
}
