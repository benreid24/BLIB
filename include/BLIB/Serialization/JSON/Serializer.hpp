#ifndef BLIB_SERIALIZATION_JSON_SERIALIZER_HPP
#define BLIB_SERIALIZATION_JSON_SERIALIZER_HPP

#include <BLIB/Serialization/JSON/JSON.hpp>
#include <BLIB/Serialization/JSON/SerializableObject.hpp>

#include <BLIB/Scripts/Function.hpp> // needed for type_traits
#include <BLIB/Scripts/Value.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

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
 * @tparam Helper for specializations, ignore
 * @ingroup JSON
 */
template<typename T, bool = (std::is_integral_v<T> || std::is_enum_v<T> ||
                             std::is_floating_point_v<T>)&&!std::is_same_v<T, bool>>
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
    static void serializeInto(Group& result, const std::string& name, const T& value);

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
        const auto* og = val.getAsGroup();
        if (og == nullptr) return false;
        const auto* f = og->getField(name);
        if (f == nullptr) return false;
        return deserialize(result, *f);
    }

    template<typename F>
    static void serializeInto(Group& group, const std::string& name, const T& val, F serialize) {
        if (group.hasField(name)) {
            BL_LOG_WARN << "Type " << typeid(T).name() << " has duplicate field " << name;
        }
        group.addField(name, serialize(val));
    }
};

} // namespace priv

template<typename T>
struct Serializer<T, false> {
    static bool deserialize(T& result, const Value& value) {
        const auto* g = value.getAsGroup();
        if (g == nullptr) return false;
        return get().deserialize(*g, &result);
    }

    static bool deserializeFrom(const Value& val, const std::string& name, T& result) {
        return priv::Serializer<T>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(const T& value) { return get().serialize(&value); }

    static void serializeInto(Group& result, const std::string& name, const T& val) {
        priv::Serializer<T>::serializeInto(result, name, val, &serialize);
    }

private:
    static const SerializableObject<T>& get() {
        static const SerializableObject<T> ser;
        return ser;
    }
};

template<>
struct Serializer<bool, false> {
    static bool deserialize(bool& result, const Value& v) {
        const bool* r = v.getAsBool();
        if (r != nullptr) {
            result = *r;
            return true;
        }
        return false;
    }

    static bool deserializeFrom(const Value& val, const std::string& name, bool& result) {
        return priv::Serializer<bool>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(bool value) { return Value(value); }

    static void serializeInto(Group& result, const std::string& name, bool value) {
        priv::Serializer<bool>::serializeInto(result, name, value, &serialize);
    }
};

template<typename T>
struct Serializer<T, true> {
    static bool deserialize(T& result, const Value& v) {
        if constexpr (std::is_enum_v<T>) {
            const long t = v.getNumericAsInteger();
            result       = static_cast<T>(t);
        }
        else if constexpr (std::is_floating_point_v<T>) {
            result = v.getNumericAsFloat();
        }
        else {
            result = v.getNumericAsInteger();
        }
        return true;
    }

    static bool deserializeFrom(const Value& val, const std::string& name, T& result) {
        return priv::Serializer<T>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(T value) {
        if constexpr (std::is_enum_v<T>) { return Value(static_cast<long>(value)); }
        else {
            return Value(value);
        }
    }

    static void serializeInto(Group& result, const std::string& name, T value) {
        priv::Serializer<T>::serializeInto(result, name, value, &serialize);
    }
};

template<>
struct Serializer<std::string, false> {
    static bool deserialize(std::string& result, const Value& v) {
        const std::string* r = v.getAsString();
        if (r != nullptr) {
            result = *r;
            return true;
        }
        return false;
    }

    static bool deserializeFrom(const Value& val, const std::string& name, std::string& result) {
        return priv::Serializer<std::string>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(const std::string& value) { return Value(value); }

    static void serializeInto(Group& result, const std::string& name, const std::string& value) {
        priv::Serializer<std::string>::serializeInto(result, name, value, &serialize);
    }
};

template<typename U, std::size_t N>
struct Serializer<U[N], false> {
    static bool deserialize(U* result, const Value& v) {
        const List* r = v.getAsList();
        if (r == nullptr) return false;
        if (r->size() != N) return false;
        for (std::size_t i = 0; i < N; ++i) {
            if (!Serializer<U>::deserialize(result[i], r->at(i))) return false;
        }
        return true;
    }

    static bool deserializeFrom(const Value& val, const std::string& name, U* result) {
        return priv::Serializer<U[N]>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(const U* val) {
        List result;
        result.reserve(N);
        for (std::size_t i = 0; i < N; ++i) {
            result.emplace_back(Serializer<U>::serialize(val[i]));
        }
        return {result};
    }

    static void serializeInto(Group& result, const std::string& name, const U* val) {
        priv::Serializer<U[N]>::serializeInto(result, name, val, &serialize);
    }
};

template<typename U>
struct Serializer<std::vector<U>, false> {
    static bool deserialize(std::vector<U>& result, const Value& v) {
        const List* r = v.getAsList();
        if (r != nullptr) {
            result.resize(r->size());
            for (unsigned int i = 0; i < r->size(); ++i) {
                if (!Serializer<U>::deserialize(result[i], r->at(i))) return false;
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

    static void serializeInto(Group& result, const std::string& name, const std::vector<U>& value) {
        priv::Serializer<std::vector<U>>::serializeInto(result, name, value, &serialize);
    }
};

template<typename U>
struct Serializer<std::unordered_set<U>, false> {
    static bool deserialize(std::unordered_set<U>& result, const Value& v) {
        const List* r = v.getAsList();
        if (r != nullptr) {
            for (unsigned int i = 0; i < r->size(); ++i) {
                U val;
                if (!Serializer<U>::deserialize(val, r->at(i))) return false;
                result.emplace(std::move(val));
            }
            return true;
        }
        return false;
    }

    static bool deserializeFrom(const Value& val, const std::string& name,
                                std::unordered_set<U>& result) {
        return priv::Serializer<std::unordered_set<U>>::deserializeFrom(
            val, name, result, &deserialize);
    }

    static Value serialize(const std::unordered_set<U>& value) {
        List result;
        result.reserve(value.size());
        for (const U& v : value) { result.push_back(Serializer<U>::serialize(v)); }
        return {result};
    }

    static void serializeInto(Group& result, const std::string& name,
                              const std::unordered_set<U>& value) {
        priv::Serializer<std::unordered_set<U>>::serializeInto(result, name, value, &serialize);
    }
};

template<typename U>
struct Serializer<std::unordered_map<std::string, U>, false> {
    static bool deserialize(std::unordered_map<std::string, U>& result, const Value& v) {
        const Group* group = v.getAsGroup();
        if (group != nullptr) {
            const Group& g = *group;
            for (const std::string& field : g.getFields()) {
                U f;
                if (!Serializer<U>::deserialize(f, *g.getField(field))) return false;
                result.emplace(field, std::move(f));
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

    static void serializeInto(Group& result, const std::string& name,
                              const std::unordered_map<std::string, U>& value) {
        priv::Serializer<std::unordered_map<std::string, U>>::serializeInto(
            result, name, value, &serialize);
    }
};

template<typename U>
struct Serializer<sf::Vector2<U>, false> {
    static Value serialize(const sf::Vector2<U>& v) {
        Group g;
        g.addField("x", Serializer<U>::serialize(v.x));
        g.addField("y", Serializer<U>::serialize(v.y));
        return {g};
    }

    static void serializeInto(const std::string& key, Group& g, const sf::Vector2<U>& val) {
        priv::Serializer<sf::Vector2<U>>::serializeInto(g, key, val, &serialize);
    }

    static bool deserialize(sf::Vector2<U>& result, const Value& val) {
        const Group* rg = val.getAsGroup();
        if (rg == nullptr) return false;
        const Group& g = *rg;
        const Value* x = g.getField("x");
        const Value* y = g.getField("y");
        if (!x || !y) return false;
        if (!Serializer<U>::deserialize(result.x, *x)) return false;
        if (!Serializer<U>::deserialize(result.y, *y)) return false;
        return true;
    }

    static bool deserializeFrom(const Value& val, const std::string& key, sf::Vector2<U>& result) {
        return priv::Serializer<sf::Vector2<U>>::deserializeFrom(val, key, result, &deserialize);
    }
};

template<typename U>
struct Serializer<sf::Vector3<U>, false> {
    static Value serialize(const sf::Vector3<U>& v) {
        Group g;
        g.addField("x", Serializer<U>::serialize(v.x));
        g.addField("y", Serializer<U>::serialize(v.y));
        g.addField("z", Serializer<U>::serialize(v.z));
        return {g};
    }

    static void serializeInto(const std::string& key, Group& g, const sf::Vector3<U>& val) {
        priv::Serializer<sf::Vector3<U>>::serializeInto(g, key, val, &serialize);
    }

    static bool deserialize(sf::Vector3<U>& result, const Value& val) {
        const Group* rg = val.getAsGroup();
        if (rg == nullptr) return false;
        const Group& g = *rg;
        const Value* x = g.getField("x");
        const Value* y = g.getField("y");
        const Value* z = g.getField("z");
        if (!x || !y || !z) return false;
        if (!Serializer<U>::deserialize(result.x, *x)) return false;
        if (!Serializer<U>::deserialize(result.y, *y)) return false;
        if (!Serializer<U>::deserialize(result.y, *z)) return false;
        return true;
    }

    static bool deserializeFrom(const Value& val, const std::string& key, sf::Vector3<U>& result) {
        return priv::Serializer<sf::Vector3<U>>::deserializeFrom(val, key, result, &deserialize);
    }
};

template<typename U>
struct Serializer<sf::Rect<U>, false> {
    static Value serialize(const sf::Rect<U>& v) {
        Group g;
        g.addField("left", Serializer<U>::serialize(v.left));
        g.addField("top", Serializer<U>::serialize(v.top));
        g.addField("width", Serializer<U>::serialize(v.width));
        g.addField("height", Serializer<U>::serialize(v.height));
        return {g};
    }

    static void serializeInto(const std::string& key, Group& g, const sf::Rect<U>& val) {
        priv::Serializer<sf::Rect<U>>::serializeInto(g, key, val, &serialize);
    }

    static bool deserialize(sf::Rect<U>& result, const Value& val) {
        const Group* rg = val.getAsGroup();
        if (rg == nullptr) return false;
        const Group& g = *rg;
        const Value* l = g.getField("left");
        const Value* t = g.getField("top");
        const Value* w = g.getField("width");
        const Value* h = g.getField("height");
        if (!l || !t || !w || !h) return false;
        if (!Serializer<U>::deserialize(result.left, *l)) return false;
        if (!Serializer<U>::deserialize(result.top, *t)) return false;
        if (!Serializer<U>::deserialize(result.width, *w)) return false;
        if (!Serializer<U>::deserialize(result.height, *h)) return false;
        return true;
    }

    static bool deserializeFrom(const Value& val, const std::string& key, sf::Rect<U>& result) {
        return priv::Serializer<sf::Rect<U>>::deserializeFrom(val, key, result, &deserialize);
    }
};

template<>
struct Serializer<script::Value, false> {
    static Value serialize(const script::Value& value) {
        const script::Value val = value.deref();

        Group g;
        g.addField("type", Serializer<script::Value::Type>::serialize(val.getType()));

        Value v(false);
        switch (val.getType()) {
        case script::Value::TBool:
            v = val.getAsBool();
            break;
        case script::Value::TNumeric:
            v = val.getAsNum();
            break;
        case script::Value::TString:
            v = val.getAsString();
            break;
        case script::Value::TArray: {
            v              = List();
            const auto arr = val.getAsArray();
            v.getAsList()->reserve(arr.size());
            for (const auto& iv : arr) { v.getAsList()->emplace_back(serialize(*iv)); }
        } break;

        case script::Value::TFunction:
        case script::Value::TVoid:
        default:
            v = "<NONSERIALIZABLE>";
            break;
        }
        g.addField("value", v);

        g.addField("props", Group());
        Group* props = g.getField("props")->getAsGroup();
        for (const auto& prop : val.allProperties()) {
            serializeInto(prop.first, *props, *prop.second);
        }

        return {g};
    }

    static void serializeInto(const std::string& name, Group& group, const script::Value& val) {
        priv::Serializer<script::Value>::serializeInto(group, name, val, &serialize);
    }

    static bool deserialize(script::Value& result, const Value& val) {
        const Group* group = val.getAsGroup();
        if (!group) return false;

        script::Value::Type type;
        if (!Serializer<script::Value::Type>::deserializeFrom(val, "type", type)) return false;

        const Value* v = group->getField("value");
        if (!v) return false;

        switch (type) {
        case script::Value::TBool:
            if (!v->getAsBool()) return false;
            result.makeBool(*v->getAsBool());
            break;

        case script::Value::TNumeric:
            if (!v->getAsInteger() && !v->getAsFloat()) return false;
            result = v->getNumericAsFloat(); // TODO - refactor int/float separated
            break;

        case script::Value::TString:
            if (!v->getAsString()) return false;
            result = *v->getAsString();
            break;

        case script::Value::TArray: {
            const List* list = v->getAsList();
            if (!list) return false;

            std::vector<script::Value> arr;
            arr.resize(list->size());
            for (unsigned int i = 0; i < list->size(); ++i) {
                if (!deserialize(arr[i], list->at(i))) return false;
            }
            result = arr;
        } break;

        case script::Value::TFunction:
        case script::Value::TVoid:
        default:
            result = false;
            break;
        }

        const Group* props = group->getGroup("props");
        if (!props) return false;

        for (const auto& prop : props->getFields()) {
            script::Value r;
            if (!deserialize(r, *props->getField(prop))) return false;
            result.setProperty(prop, r);
        }

        return true;
    }

    static bool deserializeFrom(const Value& val, const std::string& key, script::Value& result) {
        return priv::Serializer<script::Value>::deserializeFrom(val, key, result, &deserialize);
    }
};

} // namespace json
} // namespace serial
} // namespace bl

#endif
