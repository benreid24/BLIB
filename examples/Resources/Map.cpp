#include "Map.hpp"

using JsonSerializer   = bl::serial::json::Serializer<Map>;
using BinarySerializer = bl::serial::binary::Serializer<Map>;

const std::string Map::NpcPath = "game_data/characters";

bool Map::loadFromJsonFile(const std::string& path) {
    std::ifstream input(path.c_str());
    return JsonSerializer::deserializeStream(input, *this);
}

bool Map::loadFromJson(std::istream& input) {
    return JsonSerializer::deserializeStream(input, *this);
}

bool Map::saveToBinary(std::ostream& output) const {
    bl::serial::StreamOutputBuffer wrapper(output);
    bl::serial::binary::OutputStream os(wrapper);
    return BinarySerializer::serialize(os, *this);
}

bool Map::loadFromBinary(std::istream& input) {
    bl::serial::StreamInputBuffer wrapper(input);
    bl::serial::binary::InputStream is(wrapper);
    return BinarySerializer::deserialize(is, *this);
}
