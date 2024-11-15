#include "Character.hpp"
#include <fstream>

using BinarySerializer = bl::serial::binary::Serializer<Character>;
using JsonSerializer   = bl::serial::json::Serializer<Character>;

bool Character::loadFromJsonFile(const std::string& path) {
    std::ifstream input(path.c_str());
    return JsonSerializer::deserializeStream(input, *this);
}

bool Character::loadFromJson(std::istream& is) {
    return JsonSerializer::deserializeStream(is, *this);
}

bool Character::saveToBinary(std::ostream& output) const {
    bl::serial::StreamOutputBuffer wrapper(output);
    bl::serial::binary::OutputStream os(wrapper);
    return BinarySerializer::serialize(os, *this);
}

bool Character::loadFromBinary(std::istream& input) {
    bl::serial::StreamInputBuffer wrapper(input);
    bl::serial::binary::InputStream is(wrapper);
    return BinarySerializer::deserialize(is, *this);
}