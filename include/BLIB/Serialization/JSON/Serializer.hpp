#ifndef BLIB_SERIALIZATION_JSON_SERIALIZER_HPP
#define BLIB_SERIALIZATION_JSON_SERIALIZER_HPP

#include <BLIB/Serialization/JSON/JSON.hpp>
#include <BLIB/Serialization/JSON/SerializableObject.hpp>

#include <cstdint>
#include <unordered_map>

namespace bl
{
namespace serial
{
namespace json
{
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
    /**
     * @brief Deserializes the given object from the json tree
     *
     * @param result The object to read into
     * @param value The JSON tree to read from
     * @return True on success, false on error
     */
    static bool deserialize(T& result, const Value& value);

    /**
     * @brief Serializes the given value into a JSON Value
     *
     * @param value The value to serialize
     * @return Value A JSON object representing the value
     */
    static Value serialize(const T& value);

private:
    static SerializableObject<T>& get();
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename T>
bool Serializer<T>::deserialize(T& result, const Value& value) {
    const auto& g = value.getAsGroup();
    if (!g.has_value()) return false;
    return get().deserialize(g.value(), &result);
}

template<typename T>
Value Serializer<T>::serialize(const T& value) {
    return get().serialize(&value);
}

template<typename T>
SerializableObject<T>& Serializer<T>::get() {
    static SerializableObject<T> ser;
    return ser;
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
} // namespace serial
} // namespace bl

#endif
