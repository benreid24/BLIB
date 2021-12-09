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
     * @brief Deserialize the given object from the given Value, which must be a group
     *
     * @param value The value to deserialize from
     * @param name The name of the field to read
     * @param result The object to read into
     * @return True on success, false on error
     */
    static bool deserializeFrom(const Value& value, const std::string& name, T& result);

    /**
     * @brief Serializes the given value into a JSON Value
     *
     * @param value The value to serialize
     * @return Value A JSON object representing the value
     */
    static Value serialize(const T& value);

    /**
     * @brief Serialize the given object into the given value
     *
     * @param result The value to serialize into. Must be a Group
     * @param name The name of the field to write to
     * @param value The value to write
     * @return True on success, false on error
     */
    static bool serializeInto(Group& result, const std::string& name, const T& value);

private:
    static SerializableObject<T>& get();
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

/// Private inline helper methods
namespace priv
{
template<typename T>
struct Serializer {
    template<typename F>
    static bool deserializeFrom(const Value& val, const std::string& name, T& result,
                                F deserialize) {
        const auto& og = val.getAsGroup();
        if (!og.has_value()) return false;
        const auto& f = og.value().getField(name);
        if (!f.has_value()) return false;
        return deserialize(result, f.value());
    }

    template<typename F>
    static bool serializeInto(Group& group, const std::string& name, const T& val, F serialize) {
        if (group.hasField(name)) {
            BL_LOG_WARN << "Type " << typeid(T).name() << " has duplicate field " << name;
        }
        group.addField(name, serialize(val));
        return true;
    }
};

} // namespace priv

template<typename T>
bool Serializer<T>::deserialize(T& result, const Value& value) {
    const auto& g = value.getAsGroup();
    if (!g.has_value()) return false;
    return get().deserialize(g.value(), &result);
}

template<typename T>
bool Serializer<T>::deserializeFrom(const Value& val, const std::string& name, T& result) {
    return priv::Serializer<T>::deserializeFrom(val, name, result, &deserialize);
}

template<typename T>
Value Serializer<T>::serialize(const T& value) {
    return get().serialize(&value);
}

template<typename T>
bool Serializer<T>::serializeInto(Group& result, const std::string& name, const T& val) {
    return priv::Serializer<T>::serializeInto(result, name, val, &serialize);
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

    static bool deserializeFrom(const Value& val, const std::string& name, bool& result) {
        return priv::Serializer<bool>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(bool value) { return Value(value); }

    static bool serializeInto(Group& result, const std::string& name, bool value) {
        return priv::Serializer<bool>::serializeInto(result, name, value, &serialize);
    }
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

    static bool deserializeFrom(const Value& val, const std::string& name, float& result) {
        return priv::Serializer<float>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(float value) { return Value(value); }

    static bool serializeInto(Group& result, const std::string& name, float value) {
        return priv::Serializer<float>::serializeInto(result, name, value, &serialize);
    }
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

    static bool deserializeFrom(const Value& val, const std::string& name, int& result) {
        return priv::Serializer<int>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(int value) { return Value(static_cast<float>(value)); }

    static bool serializeInto(Group& result, const std::string& name, int value) {
        return priv::Serializer<int>::serializeInto(result, name, value, &serialize);
    }
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

    static bool deserializeFrom(const Value& val, const std::string& name, std::string& result) {
        return priv::Serializer<std::string>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(const std::string& value) { return Value(value); }

    static bool serializeInto(Group& result, const std::string& name, const std::string& value) {
        return priv::Serializer<std::string>::serializeInto(result, name, value, &serialize);
    }
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

    static bool deserializeFrom(const Value& val, const std::string& name, std::vector<U>& result) {
        return priv::Serializer<std::vector<U>>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(const std::vector<U>& value) {
        List result;
        result.reserve(value.size());
        for (const U& v : value) { result.push_back(Serializer<U>::serialize(v)); }
        return {result};
    }

    static bool serializeInto(Group& result, const std::string& name, const std::vector<U>& value) {
        return priv::Serializer<std::vector<U>>::serializeInto(result, name, value, &serialize);
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

    static bool deserializeFrom(const Value& val, const std::string& name,
                                std::unordered_map<std::string, U>& result) {
        return priv::Serializer<std::unordered_map<std::string, U>>::deserializeFrom(
            val, name, result, &deserialize);
    }

    static Value serialize(const std::unordered_map<std::string, U>& value) {
        Group result;
        for (const auto& p : value) {
            result.addField(p.first, Serializer<U>::serialize(p.second));
        }
        return {result};
    }

    static bool serializeInto(Group& result, const std::string& name,
                              const std::unordered_map<std::string, U>& value) {
        return priv::Serializer<std::unordered_map<std::string, U>>::serializeInto(
            result, name, value, &serialize);
    }
};

} // namespace json
} // namespace serial
} // namespace bl

#endif
