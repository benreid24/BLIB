#include <BLIB/Files/JSON/SerializableObject.hpp>
#include <BLIB/Files/JSON/Serializer.hpp>

namespace bl
{
namespace json
{
template<>
bool Serializer::deserialize(std::string& result, const Value& v) {
    String r = v.getAsString();
    if (r.has_value()) {
        result = r.value();
        return true;
    }
    return false;
}

template<>
bool Serializer::deserialize(bool& result, const Value& v) {
    Bool r = v.getAsBool();
    if (r.has_value()) {
        result = r.value();
        return true;
    }
    return false;
}

template<>
bool Serializer::deserialize(SerializableObject& result, const Value& v) {
    RGroup group = v.getAsGroup();
    if (!group.has_value()) return false;
    return result.deserialize(group.value());
}

template<>
Value Serializer::serialize(const std::string& value) {
    return {value};
}

template<>
Value Serializer::serialize(const bool& value) {
    return Value(value);
}

template<>
Value Serializer::serialize(const SerializableObject& value) {
    return {value.serialize()};
}

} // namespace json
} // namespace bl
