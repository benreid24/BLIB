#ifndef BLIB_FILES_JSON_SERIALIZER_HPP
#define BLIB_FILES_JSON_SERIALIZER_HPP

#include <BLIB/Files/JSON/JSON.hpp>

#include <cstdint>
#include <unordered_map>

namespace bl
{
namespace file
{
namespace json
{
class SerializableObject;

/**
 * @brief Helper class for SerializableField. Specializations are provided for json native types,
 *        vector, and unordered_map. User defined types should either implement SerializableObject
 *        or provide a specialization of this class
 *
 * @tparam T The type to serialize
 * @ingroup JSON
 */
template<typename T>
struct Serializer {
    static bool deserialize(T& result, const Value& value);

    static Value serialize(const T& value);
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename T>
bool Serializer<T>::deserialize(T& result, const Value& value) {
    RGroup group = value.getAsGroup();
    if (!group.has_value()) return false;
    return result.deserialize(group.value());
}

template<typename T>
Value Serializer<T>::serialize(const T& value) {
    return {value.serialize()};
}

template<>
struct Serializer<bool> {
    static bool deserialize(bool& result, const Value& v) {
        Bool r = v.getAsBool();
        if (r.has_value()) {
            result = r.value();
            return true;
        }
        return false;
    }

    static Value serialize(bool value) { return Value(value); }
};

template<>
struct Serializer<float> {
    static bool deserialize(float& result, const Value& v) {
        Numeric r = v.getAsNumeric();
        if (r.has_value()) {
            result = r.value();
            return true;
        }
        return false;
    }

    static Value serialize(float value) { return Value(value); }
};

template<>
struct Serializer<int> {
    static bool deserialize(int& result, const Value& v) {
        Numeric r = v.getAsNumeric();
        if (r.has_value()) {
            result = r.value();
            return true;
        }
        return false;
    }

    static Value serialize(int value) { return Value(static_cast<float>(value)); }
};

template<>
struct Serializer<std::string> {
    static bool deserialize(std::string& result, const Value& v) {
        String r = v.getAsString();
        if (r.has_value()) {
            result = r.value();
            return true;
        }
        return false;
    }

    static Value serialize(const std::string& value) { return Value(value); }
};

template<typename U>
struct Serializer<std::vector<U>> {
    static bool deserialize(std::vector<U>& result, const Value& v) {
        RList r = v.getAsList();
        if (r.has_value()) {
            result.resize(r.value().size());
            for (unsigned int i = 0; i < r.value().size(); ++i) {
                if (!Serializer<U>::deserialize(result[i], r.value()[i])) return false;
            }
            return true;
        }
        return false;
    }

    static Value serialize(const std::vector<U>& value) {
        List result;
        result.reserve(value.size());
        for (const U& v : value) { result.push_back(Serializer<U>::serialize(v)); }
        return {result};
    }
};

template<typename U>
struct Serializer<std::unordered_map<std::string, U>> {
    static bool deserialize(std::unordered_map<std::string, U>& result, const Value& v) {
        RGroup group = v.getAsGroup();
        if (group.has_value()) {
            const Group& g = group.value();
            for (const std::string& field : g.getFields()) {
                U f;
                if (!Serializer<U>::deserialize(f, g.getField(field).value())) return false;
                result[field] = f;
            }
            return true;
        }
        return false;
    }

    static Value serialize(const std::unordered_map<std::string, U>& value) {
        Group result;
        for (const auto& p : value) {
            result.addField(p.first, Serializer<U>::serialize(p.second));
        }
        return {result};
    }
};

} // namespace json
} // namespace file
} // namespace bl

#endif
