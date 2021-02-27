#ifndef BLIB_FILES_JSON_SERIALIZER_HPP
#define BLIB_FILES_JSON_SERIALIZER_HPP

#include <BLIB/Files/JSON/JSON.hpp>

#include <type_traits>
#include <unordered_map>

namespace bl
{
namespace json
{
class SerializableObject;

struct Serializer {
    template<typename T, class Enabler = void>
    static bool deserialize(T& result, const Value& value);

    template<typename T>
    static bool deserialize(std::vector<T>& result, const Value& value);

    template<typename T>
    static bool deserialize(std::unordered_map<std::string, T>& result, const Value& value);

    template<typename T, class Enabler = void>
    static Value serialize(const T& value);

    template<typename T>
    static Value serialize(const std::vector<T>& value);

    template<typename T>
    static Value serialize(const std::unordered_map<std::string, T>& value);
};

///////////////////////// BUILT-IN SPECIALIZATIONS ////////////////////////////////

template<typename T, class = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
bool Serializer::deserialize(T& result, const Value& v) {
    Numeric r = v.getAsNumeric();
    if (r.has_value()) {
        result = r.value();
        return true;
    }
    return false;
}

template<>
bool Serializer::deserialize(std::string& result, const Value& v);

template<>
bool Serializer::deserialize(bool& result, const Value& v);

template<>
bool Serializer::deserialize(SerializableObject& result, const Value& v);

template<typename T>
bool Serializer::deserialize(std::vector<T>& result, const Value& v) {
    RList r = v.getAsList();
    if (r.has_value()) {
        result.resize(r.value().size());
        for (unsigned int i = 0; i < r.value().size(); ++i) {
            if (!deserialize(result[i], r.value()[i])) return false;
        }
        return true;
    }
    return false;
}

template<typename T>
bool Serializer::deserialize(std::unordered_map<std::string, T>& result, const Value& v) {
    RGroup group = v.getAsGroup();
    if (group.has_value()) {
        const Group& g = group.value();
        for (const std::string& field : g.getFields()) {
            T f;
            if (!deserialize(f, g.getField(field).value())) return false;
            result[field] = f;
        }
        return true;
    }
    return false;
}

template<typename T, class = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
Value Serializer::serialize(const T& value) {
    return Value(static_cast<float>(value));
}

template<>
Value Serializer::serialize(const std::string& value);

template<>
Value Serializer::serialize(const bool& value);

template<>
Value Serializer::serialize(const SerializableObject& value);

template<typename T>
Value Serializer::serialize(const std::vector<T>& value) {
    List result;
    result.reserve(value.size());
    for (const T& v : value) { result.push_back(serialize(v)); }
    return {result};
}

template<typename T>
Value Serializer::serialize(const std::unordered_map<std::string, T>& value) {
    Group result;
    for (const auto& p : value) { result.addField(p.first, serialize(p.second)); }
    return {result};
}

} // namespace json
} // namespace bl

#endif
