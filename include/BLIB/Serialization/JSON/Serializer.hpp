#ifndef BLIB_SERIALIZATION_JSON_SERIALIZER_HPP
#define BLIB_SERIALIZATION_JSON_SERIALIZER_HPP

#include <BLIB/Serialization/JSON/JSON.hpp>
#include <BLIB/Serialization/JSON/JSONLoader.hpp>
#include <BLIB/Serialization/SerializableObject.hpp>

#include <BLIB/Containers/Vector2d.hpp>
#include <BLIB/Logging.hpp>
#include <BLIB/Scripts.hpp>
#include <BLIB/Util/StreamUtil.hpp>
#include <BLIB/Util/UnderlyingType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <array>
#include <cstdint>
#include <glm/glm.hpp>
#include <unordered_map>
#include <unordered_set>
#include <variant>

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
     * @brief Reads the value directly from the given stream instead of decoding into JSON objects
     *        first. More memory and time efficient
     *
     * @param stream The stream to read from
     * @param result The value to read into
     * @return True on success, false on error
     */
    static bool deserializeStream(std::istream& stream, T& result);

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

    /**
     * @brief Serializes the given value directly to the given stream instead of creating
     *        intermediate JSON objects first
     *
     * @param stream The stream to write to
     * @param value The value to write
     * @param tabSize Spaces to indent per level
     * @param currentIndent Current level of indentation in spaces
     * @return True on success, false on error
     */
    static bool serializeStream(std::ostream& stream, const T& value, unsigned int tabSize,
                                unsigned int currentIndent = 0);
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
        return SerializableObjectBase::get<T>().deserializeJSON(*g, &result);
    }

    static bool deserializeFrom(const Value& val, const std::string& name, T& result) {
        return priv::Serializer<T>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(const T& value) {
        return SerializableObjectBase::get<T>().serializeJSON(&value);
    }

    static void serializeInto(Group& result, const std::string& name, const T& val) {
        priv::Serializer<T>::serializeInto(result, name, val, &serialize);
    }

    static bool deserializeStream(std::istream& stream, T& result) {
        return SerializableObjectBase::get<T>().deserializeJsonStream(stream, &result);
    }

    static bool serializeStream(std::ostream& stream, const T& value, unsigned int tabSize,
                                unsigned int currentIndent) {
        return SerializableObjectBase::get<T>().serializeJsonStream(
            stream, &value, tabSize, currentIndent);
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

    static bool deserializeStream(std::istream& stream, bool& result) {
        json::Loader loader(stream);
        return loader.loadBool(result);
    }

    static bool serializeStream(std::ostream& stream, bool value, unsigned int, unsigned int) {
        stream << (value ? "true" : "false");
        return stream.good();
    }
};

template<typename T>
struct Serializer<T, true> {
    static bool deserialize(T& result, const Value& v) {
        if constexpr (std::is_enum_v<T>) {
            const long t = v.getNumericAsInteger();
            result       = static_cast<T>(t);
        }
        else if constexpr (std::is_floating_point_v<T>) { result = v.getNumericAsFloat(); }
        else { result = v.getNumericAsInteger(); }
        return true;
    }

    static bool deserializeFrom(const Value& val, const std::string& name, T& result) {
        return priv::Serializer<T>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(T value) {
        if constexpr (std::is_enum_v<T>) { return Value(static_cast<long>(value)); }
        else { return Value(value); }
    }

    static void serializeInto(Group& result, const std::string& name, T value) {
        priv::Serializer<T>::serializeInto(result, name, value, &serialize);
    }

    static bool deserializeStream(std::istream& stream, T& result) {
        json::Loader loader(stream);
        Value v(0);
        if (!loader.loadNumeric(v)) return false;
        if constexpr (std::is_enum_v<T>) {
            const long t = v.getNumericAsInteger();
            result       = static_cast<T>(t);
        }
        else if constexpr (std::is_floating_point_v<T>) { result = v.getNumericAsFloat(); }
        else { result = v.getNumericAsInteger(); }
        return true;
    }

    static bool serializeStream(std::ostream& stream, T value, unsigned int, unsigned int) {
        if constexpr (std::is_enum_v<T>) {
            using U = std::underlying_type_t<T>;
            if constexpr (std::is_same_v<U, std::uint8_t>) {
                stream << static_cast<unsigned int>(value);
            }
            else if constexpr (std::is_same_v<U, std::int8_t>) {
                stream << static_cast<int>(value);
            }
            else { stream << static_cast<U>(value); }
        }
        else if constexpr (std::is_floating_point_v<T>) { stream << std::fixed << value; }
        else if constexpr (std::is_same_v<T, std::uint8_t>) {
            stream << static_cast<unsigned int>(value);
        }
        else if constexpr (std::is_same_v<T, std::int8_t>) { stream << static_cast<int>(value); }
        else { stream << value; }
        return stream.good();
    }
};

template<typename U>
struct Serializer<U*, false> {
    using T = U*;

    static bool deserialize(T result, const Value& v) {
        if (!result) return false;
        return Serializer<U>::deserialize(*result, v);
    }

    static bool deserializeFrom(const Value& val, const std::string& name, T result) {
        if (!result) return false;
        return Serializer<U>::deserializeFrom(val, name, *result);
    }

    static Value serialize(const T value) {
        if (!value) return false;
        return Serializer<U>::serialize(*value);
    }

    static void serializeInto(Group& result, const std::string& name, const T value) {
        if (!value) return;
        Serializer<U>::serializeInto(result, name, *value);
    }

    static bool deserializeStream(std::istream& stream, T result) {
        if (!result) return false;
        return Serializer<U>::deserializeStream(stream, *result);
    }

    static bool serializeStream(std::ostream& stream, const T value, unsigned int tabSize,
                                unsigned int currentIndent) {
        if (!value) return false;
        return Serializer<U>::serializeStream(stream, *value, tabSize, currentIndent);
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

    static bool deserializeStream(std::istream& stream, std::string& result) {
        json::Loader loader(stream);
        return loader.loadString(result);
    }

    static bool serializeStream(std::ostream& stream, const std::string& value, unsigned int,
                                unsigned int) {
        stream << '"';
        for (const char c : value) {
            switch (c) {
            case '\n':
                stream << "\\n";
                break;
            case '\t':
                stream << "\\t";
                break;
            case '"':
                stream << "\\\"";
                break;
            default:
                stream << c;
                break;
            }
        }
        stream << '"';
        return stream.good();
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

    static bool deserializeStream(std::istream& stream, U* result) {
        if (!util::StreamUtil::skipWhitespace(stream)) return false;
        if (stream.get() != '[') return false;
        if (!util::StreamUtil::skipWhitespace(stream)) return false;

        unsigned int i = 0;
        while (i < N) {
            if (!Serializer<U>::deserializeStream(stream, result[i])) return false;
            ++i;
            if (!util::StreamUtil::skipWhitespace(stream)) return false;
            switch (stream.get()) {
            case ',':
                if (i >= N) return false;
                break;
            case ']':
                return i == N;
            default:
                return false;
            }
        }

        return false;
    }

    static bool serializeStream(std::ostream& stream, const U* value, unsigned int tabSize,
                                unsigned int currentIndent) {
        stream << "[";
        if (tabSize > 0) stream << "\n";

        const unsigned int sc = currentIndent + tabSize;
        for (unsigned int i = 0; i < N; ++i) {
            util::StreamUtil::writeRepeated(stream, ' ', sc);
            if (!Serializer<U>::serializeStream(stream, value[i], tabSize, sc)) return false;
            if (i < N - 1) stream << ",";
            if (tabSize > 0) stream << '\n';
        }

        util::StreamUtil::writeRepeated(stream, ' ', currentIndent);
        stream << ']';
        return stream.good();
    }
};

template<typename U, std::size_t N>
struct Serializer<std::array<U, N>, false> {
    using T = std::array<U, N>;

    static bool deserialize(T& result, const Value& v) {
        const List* r = v.getAsList();
        if (r == nullptr) return false;
        if (r->size() != N) return false;
        for (std::size_t i = 0; i < N; ++i) {
            if (!Serializer<U>::deserialize(result[i], r->at(i))) return false;
        }
        return true;
    }

    static bool deserializeFrom(const Value& val, const std::string& name, T& result) {
        return priv::Serializer<U[N]>::deserializeFrom(val, name, result, &deserialize);
    }

    static Value serialize(const T& val) {
        List result;
        result.reserve(N);
        for (std::size_t i = 0; i < N; ++i) {
            result.emplace_back(Serializer<U>::serialize(val[i]));
        }
        return {result};
    }

    static void serializeInto(Group& result, const std::string& name, const T& val) {
        priv::Serializer<U[N]>::serializeInto(result, name, val, &serialize);
    }

    static bool deserializeStream(std::istream& stream, T& result) {
        if (!util::StreamUtil::skipWhitespace(stream)) return false;
        if (stream.get() != '[') return false;
        if (!util::StreamUtil::skipWhitespace(stream)) return false;

        unsigned int i = 0;
        while (i < N) {
            if (!Serializer<U>::deserializeStream(stream, result[i])) return false;
            ++i;
            if (!util::StreamUtil::skipWhitespace(stream)) return false;
            switch (stream.get()) {
            case ',':
                if (i >= N) return false;
                break;
            case ']':
                return i == N;
            default:
                return false;
            }
        }

        return false;
    }

    static bool serializeStream(std::ostream& stream, const T& value, unsigned int tabSize,
                                unsigned int currentIndent) {
        stream << "[";
        if (tabSize > 0) stream << "\n";

        const unsigned int sc = currentIndent + tabSize;
        for (unsigned int i = 0; i < N; ++i) {
            util::StreamUtil::writeRepeated(stream, ' ', sc);
            if (!Serializer<U>::serializeStream(stream, value[i], tabSize, sc)) return false;
            if (i < N - 1) stream << ",";
            if (tabSize > 0) stream << '\n';
        }

        util::StreamUtil::writeRepeated(stream, ' ', currentIndent);
        stream << ']';
        return stream.good();
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

    static bool deserializeStream(std::istream& stream, std::vector<U>& result) {
        if (!util::StreamUtil::skipWhitespace(stream)) return false;
        if (stream.get() != '[') return false;
        if (!util::StreamUtil::skipWhitespace(stream)) return false;

        while (stream.good()) {
            if (stream.peek() == ']') {
                stream.get();
                return true;
            }
            result.emplace_back();
            if (!Serializer<U>::deserializeStream(stream, result.back())) return false;
            if (!util::StreamUtil::skipWhitespace(stream)) return false;
            switch (stream.get()) {
            case ',':
                break;
            case ']':
                return true;
            default:
                return false;
            }
        }

        return false;
    }

    static bool serializeStream(std::ostream& stream, const std::vector<U>& value,
                                unsigned int tabSize, unsigned int currentIndent) {
        stream << "[";
        if (tabSize > 0) stream << "\n";

        const unsigned int sc = currentIndent + tabSize;
        for (unsigned int i = 0; i < value.size(); ++i) {
            util::StreamUtil::writeRepeated(stream, ' ', sc);
            if (!Serializer<U>::serializeStream(stream, value[i], tabSize, sc)) return false;
            if (i < value.size() - 1) stream << ",";
            if (tabSize > 0) stream << '\n';
        }

        util::StreamUtil::writeRepeated(stream, ' ', currentIndent);
        stream << ']';
        return stream.good();
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

    static bool deserializeStream(std::istream& stream, std::unordered_set<U>& result) {
        if (!util::StreamUtil::skipWhitespace(stream)) return false;
        if (stream.get() != '[') return false;
        if (!util::StreamUtil::skipWhitespace(stream)) return false;

        while (stream.good()) {
            U temp;
            if (!Serializer<U>::deserializeStream(stream, temp)) return false;
            result.emplace(std::move(temp));
            if (!util::StreamUtil::skipWhitespace(stream)) return false;
            switch (stream.get()) {
            case ',':
                break;
            case ']':
                return true;
            default:
                return false;
            }
        }

        return false;
    }

    static bool serializeStream(std::ostream& stream, const std::unordered_set<U>& value,
                                unsigned int tabSize, unsigned int currentIndent) {
        stream << "[";
        if (tabSize > 0) stream << "\n";

        const unsigned int sc = currentIndent + tabSize;
        unsigned int i        = 0;
        for (const U& val : value) {
            util::StreamUtil::writeRepeated(stream, ' ', sc);
            if (!Serializer<U>::serializeStream(stream, val, tabSize, sc)) return false;
            if (i < value.size() - 1) stream << ",";
            if (tabSize > 0) stream << '\n';
            ++i;
        }

        util::StreamUtil::writeRepeated(stream, ' ', sc);
        stream << ']';
        return stream.good();
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

    static bool deserializeStream(std::istream& stream,
                                  std::unordered_map<std::string, U>& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) return false;
        return deserialize(result, val);
    }

    static bool serializeStream(std::ostream& stream,
                                const std::unordered_map<std::string, U>& value,
                                unsigned int tabSize, unsigned int currentIndent) {
        stream << "{";
        if (tabSize > 0) stream << "\n";

        const unsigned int sc = tabSize + currentIndent;
        unsigned int i        = 0;
        for (const auto& pair : value) {
            if (!util::StreamUtil::writeRepeated(stream, ' ', sc)) return false;
            stream << '"' << pair.first << "\": ";
            if (!Serializer<U>::serializeStream(stream, pair.second, tabSize, sc)) return false;
            if (i < value.size() - 1) stream << ',';
            if (tabSize > 0) stream << "\n";
            ++i;
        }
        util::StreamUtil::writeRepeated(stream, ' ', currentIndent);
        stream << "}";
        return stream.good();
    }
};

template<typename U, typename V>
struct Serializer<std::unordered_map<U, V>, false> {
    static_assert(std::is_integral_v<U> || std::is_enum_v<U>, "Map must have integer keys");
    using T = util::UnderlyingTypeT<U>;

    static bool deserialize(std::unordered_map<U, V>& result, const Value& v) {
        const Group* group = v.getAsGroup();
        if (group != nullptr) {
            const Group& g = *group;
            for (const std::string& field : g.getFields()) {
                const U k = static_cast<U>(static_cast<T>(std::stoll(field)));
                V f;
                if (!Serializer<V>::deserialize(f, *g.getField(field))) return false;
                result.emplace(k, std::move(f));
            }
            return true;
        }
        return false;
    }

    static bool deserializeFrom(const Value& val, const std::string& name,
                                std::unordered_map<U, V>& result) {
        return priv::Serializer<std::unordered_map<U, V>>::deserializeFrom(
            val, name, result, &deserialize);
    }

    static Value serialize(const std::unordered_map<U, V>& value) {
        Group result;
        for (const auto& p : value) {
            result.addField(std::to_string(static_cast<T>(p.first)),
                            Serializer<V>::serialize(p.second));
        }
        return {result};
    }

    static void serializeInto(Group& result, const std::string& name,
                              const std::unordered_map<U, V>& value) {
        priv::Serializer<std::unordered_map<U, V>>::serializeInto(result, name, value, &serialize);
    }

    static bool deserializeStream(std::istream& stream, std::unordered_map<U, V>& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) return false;
        return deserialize(result, val);
    }

    static bool serializeStream(std::ostream& stream, const std::unordered_map<U, V>& value,
                                unsigned int tabSize, unsigned int currentIndent) {
        stream << "{";
        if (tabSize > 0) stream << "\n";

        const unsigned int sc = tabSize + currentIndent;
        unsigned int i        = 0;
        for (const auto& pair : value) {
            if (!util::StreamUtil::writeRepeated(stream, ' ', sc)) return false;
            stream << '"' << pair.first << "\": ";
            if (!Serializer<V>::serializeStream(stream, pair.second, tabSize, sc)) return false;
            if (i < value.size() - 1) stream << ',';
            if (tabSize > 0) stream << "\n";
            ++i;
        }
        util::StreamUtil::writeRepeated(stream, ' ', currentIndent);
        stream << "}";
        return stream.good();
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

    static bool deserializeStream(std::istream& stream, sf::Vector2<U>& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) return false;
        return deserialize(result, val);
    }

    static bool serializeStream(std::ostream& stream, const sf::Vector2<U>& value, unsigned int,
                                unsigned int) {
        stream << "{ ";
        stream << "\"x\": ";
        Serializer<U>::serializeStream(stream, value.x, 0, 0);
        stream << ", \"y\": ";
        Serializer<U>::serializeStream(stream, value.y, 0, 0);
        stream << " }";
        return stream.good();
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

    static bool deserializeStream(std::istream& stream, sf::Vector3<U>& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) return false;
        return deserialize(result, val);
    }

    static bool serializeStream(std::ostream& stream, const sf::Vector3<U>& value, unsigned int,
                                unsigned int) {
        stream << "{ ";
        stream << "\"x\": ";
        Serializer<U>::serializeStream(stream, value.x, 0, 0);
        stream << ", \"y\": ";
        Serializer<U>::serializeStream(stream, value.y, 0, 0);
        stream << ", \"z\": ";
        Serializer<U>::serializeStream(stream, value.z, 0, 0);
        stream << " }";
        return stream.good();
    }
};

template<typename U, glm::qualifier P>
struct Serializer<glm::vec<2, U, P>, false> {
    using V = glm::vec<2, U, P>;

    static Value serialize(const V& v) {
        Group g;
        g.addField("x", Serializer<U>::serialize(v.x));
        g.addField("y", Serializer<U>::serialize(v.y));
        return {g};
    }

    static void serializeInto(const std::string& key, Group& g, const V& val) {
        priv::Serializer<V>::serializeInto(g, key, val, &serialize);
    }

    static bool deserialize(V& result, const Value& val) {
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

    static bool deserializeFrom(const Value& val, const std::string& key, V& result) {
        return priv::Serializer<V>::deserializeFrom(val, key, result, &deserialize);
    }

    static bool deserializeStream(std::istream& stream, V& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) return false;
        return deserialize(result, val);
    }

    static bool serializeStream(std::ostream& stream, const V& value, unsigned int, unsigned int) {
        stream << "{ ";
        stream << "\"x\": ";
        Serializer<U>::serializeStream(stream, value.x, 0, 0);
        stream << ", \"y\": ";
        Serializer<U>::serializeStream(stream, value.y, 0, 0);
        stream << " }";
        return stream.good();
    }
};

template<typename U, glm::qualifier P>
struct Serializer<glm::vec<3, U, P>, false> {
    using V = glm::vec<3, U, P>;

    static Value serialize(const V& v) {
        Group g;
        g.addField("x", Serializer<U>::serialize(v.x));
        g.addField("y", Serializer<U>::serialize(v.y));
        g.addField("z", Serializer<U>::serialize(v.z));
        return {g};
    }

    static void serializeInto(const std::string& key, Group& g, const V& val) {
        priv::Serializer<V>::serializeInto(g, key, val, &serialize);
    }

    static bool deserialize(V& result, const Value& val) {
        const Group* rg = val.getAsGroup();
        if (rg == nullptr) return false;
        const Group& g = *rg;
        const Value* x = g.getField("x");
        const Value* y = g.getField("y");
        const Value* z = g.getField("z");
        if (!x || !y || !z) return false;
        if (!Serializer<U>::deserialize(result.x, *x)) return false;
        if (!Serializer<U>::deserialize(result.y, *y)) return false;
        if (!Serializer<U>::deserialize(result.z, *z)) return false;
        return true;
    }

    static bool deserializeFrom(const Value& val, const std::string& key, V& result) {
        return priv::Serializer<V>::deserializeFrom(val, key, result, &deserialize);
    }

    static bool deserializeStream(std::istream& stream, V& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) return false;
        return deserialize(result, val);
    }

    static bool serializeStream(std::ostream& stream, const V& value, unsigned int, unsigned int) {
        stream << "{ ";
        stream << "\"x\": ";
        Serializer<U>::serializeStream(stream, value.x, 0, 0);
        stream << ", \"y\": ";
        Serializer<U>::serializeStream(stream, value.y, 0, 0);
        stream << ", \"z\": ";
        Serializer<U>::serializeStream(stream, value.z, 0, 0);
        stream << " }";
        return stream.good();
    }
};

template<typename U, glm::qualifier P>
struct Serializer<glm::vec<4, U, P>, false> {
    using V = glm::vec<4, U, P>;

    static Value serialize(const V& v) {
        Group g;
        g.addField("x", Serializer<U>::serialize(v.x));
        g.addField("y", Serializer<U>::serialize(v.y));
        g.addField("z", Serializer<U>::serialize(v.z));
        g.addField("w", Serializer<U>::serialize(v.w));
        return {g};
    }

    static void serializeInto(const std::string& key, Group& g, const V& val) {
        priv::Serializer<V>::serializeInto(g, key, val, &serialize);
    }

    static bool deserialize(V& result, const Value& val) {
        const Group* rg = val.getAsGroup();
        if (rg == nullptr) return false;
        const Group& g = *rg;
        const Value* x = g.getField("x");
        const Value* y = g.getField("y");
        const Value* z = g.getField("z");
        const Value* w = g.getField("w");
        if (!x || !y || !z || !w) return false;
        if (!Serializer<U>::deserialize(result.x, *x)) return false;
        if (!Serializer<U>::deserialize(result.y, *y)) return false;
        if (!Serializer<U>::deserialize(result.z, *z)) return false;
        if (!Serializer<U>::deserialize(result.w, *w)) return false;
        return true;
    }

    static bool deserializeFrom(const Value& val, const std::string& key, V& result) {
        return priv::Serializer<V>::deserializeFrom(val, key, result, &deserialize);
    }

    static bool deserializeStream(std::istream& stream, V& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) return false;
        return deserialize(result, val);
    }

    static bool serializeStream(std::ostream& stream, const V& value, unsigned int, unsigned int) {
        stream << "{ ";
        stream << "\"x\": ";
        Serializer<U>::serializeStream(stream, value.x, 0, 0);
        stream << ", \"y\": ";
        Serializer<U>::serializeStream(stream, value.y, 0, 0);
        stream << ", \"z\": ";
        Serializer<U>::serializeStream(stream, value.z, 0, 0);
        stream << ", \"w\": ";
        Serializer<U>::serializeStream(stream, value.w, 0, 0);
        stream << " }";
        return stream.good();
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

    static bool deserializeStream(std::istream& stream, sf::Rect<U>& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) return false;
        return deserialize(result, val);
    }

    static bool serializeStream(std::ostream& stream, const sf::Rect<U>& value, unsigned int,
                                unsigned int) {
        stream << "{ ";
        stream << "\"left\": ";
        Serializer<U>::serializeStream(stream, value.left, 0, 0);
        stream << ", \"top\": ";
        Serializer<U>::serializeStream(stream, value.top, 0, 0);
        stream << ", \"width\": ";
        Serializer<U>::serializeStream(stream, value.width, 0, 0);
        stream << ", \"height\": ";
        Serializer<U>::serializeStream(stream, value.height, 0, 0);
        stream << " }";
        return stream.good();
    }
};

template<>
struct Serializer<script::Value, false> {
    static Value serialize(const script::Value& value) {
        const auto& val = value.value();

        Group g;
        g.addField("type", Serializer<script::PrimitiveValue::Type>::serialize(val.getType()));

        Value v(false);
        switch (val.getType()) {
        case script::PrimitiveValue::TBool:
            v = val.getAsBool();
            break;
        case script::PrimitiveValue::TInteger:
            v = val.getAsInt();
            break;
        case script::PrimitiveValue::TFloat:
            v = val.getAsFloat();
            break;
        case script::PrimitiveValue::TString:
            v = val.getAsString();
            break;
        case script::PrimitiveValue::TArray: {
            v               = List();
            const auto& arr = val.getAsArray();
            v.getAsList()->reserve(arr.size());
            for (const auto& iv : arr) { v.getAsList()->emplace_back(serialize(iv)); }
        } break;

        case script::PrimitiveValue::TFunction:
        case script::PrimitiveValue::TVoid:
        default:
            v = "<NONSERIALIZABLE>";
            break;
        }
        g.addField("value", v);

        Group props;
        for (const auto& prop : value.allProperties()) {
            props.addField(prop.first, serialize(prop.second.deref()));
        }
        g.addField("props", props);

        return {g};
    }

    static void serializeInto(const std::string& name, Group& group, const script::Value& val) {
        priv::Serializer<script::Value>::serializeInto(group, name, val, &serialize);
    }

    static bool deserialize(script::Value& result, const Value& val) {
        try {
            const Group* group = val.getAsGroup();
            if (!group) return false;

            script::PrimitiveValue::Type type;
            if (!Serializer<script::PrimitiveValue::Type>::deserializeFrom(val, "type", type)) {
                return false;
            }

            const Value* v = group->getField("value");
            if (!v) return false;

            switch (type) {
            case script::PrimitiveValue::TBool:
                if (!v->getAsBool()) return false;
                result.value() = *v->getAsBool();
                break;

            case script::PrimitiveValue::TInteger:
                if (!v->getAsInteger() && !v->getAsFloat()) return false;
                result.value() = v->getNumericAsInteger();
                break;

            case script::PrimitiveValue::TFloat:
                if (!v->getAsInteger() && !v->getAsFloat()) return false;
                result.value() = v->getNumericAsFloat();
                break;

            case script::PrimitiveValue::TString:
                if (!v->getAsString()) return false;
                result.value() = *v->getAsString();
                break;

            case script::PrimitiveValue::TArray: {
                const List* list = v->getAsList();
                if (!list) return false;

                std::vector<script::Value> arr;
                arr.resize(list->size());
                for (unsigned int i = 0; i < list->size(); ++i) {
                    if (!deserialize(arr[i], list->at(i))) return false;
                }
                result = arr;
            } break;

            case script::PrimitiveValue::TRef:
            case script::PrimitiveValue::TFunction:
            case script::PrimitiveValue::TVoid:
            default:
                result.value() = false;
                break;
            }

            const Group* props = group->getGroup("props");
            if (props) {
                for (const std::string& prop : props->getFields()) {
                    script::Value pval;
                    if (!deserialize(pval, *props->getField(prop))) return false;
                    result.setProperty(prop, {std::move(pval)});
                }
            }
        } catch (...) { return false; }

        return true;
    }

    static bool deserializeFrom(const Value& val, const std::string& key, script::Value& result) {
        return priv::Serializer<script::Value>::deserializeFrom(val, key, result, &deserialize);
    }

    static bool deserializeStream(std::istream& stream, script::Value& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) return false;
        return deserialize(result, val);
    }

    static bool serializeStream(std::ostream& stream, const script::Value& value, unsigned int,
                                unsigned int) {
        const Value val = serialize(value);
        stream << val;
        return stream.good();
    }
};

template<typename U>
struct Serializer<ctr::Vector2D<U>, false> {
    static Value serialize(const ctr::Vector2D<U>& v) {
        Group g;
        g.addField("w", Serializer<unsigned int>::serialize(v.getWidth()));
        g.addField("h", Serializer<unsigned int>::serialize(v.getHeight()));
        List data;
        data.reserve(v.getHeight() * v.getWidth());
        for (unsigned int x = 0; x < v.getWidth(); ++x) {
            for (unsigned int y = 0; y < v.getHeight(); ++y) {
                data.emplace_back(Serializer<U>::serialize(v(x, y)));
            }
        }
        g.addField("data", {data});
        return {g};
    }

    static void serializeInto(const std::string& key, Group& g, const ctr::Vector2D<U>& val) {
        priv::Serializer<ctr::Vector2D<U>>::serializeInto(g, key, val, &serialize);
    }

    static bool deserialize(ctr::Vector2D<U>& result, const Value& val) {
        const Group* rg = val.getAsGroup();
        if (rg == nullptr) return false;
        const Group& g    = *rg;
        const Value* w    = g.getField("w");
        const Value* h    = g.getField("h");
        const Value* data = g.getField("data");
        if (!w || !h || !data) return false;
        if (w->getType() != Value::Type::Integer || h->getType() != Value::Type::Integer ||
            data->getType() != Value::Type::List) {
            return false;
        }
        const unsigned int mx = *w->getAsInteger();
        const unsigned int my = *h->getAsInteger();
        const List& d         = *data->getAsList();
        if (d.size() != mx * my) return false;
        result.setSize(mx, my);
        for (unsigned int x = 0; x < mx; ++x) {
            for (unsigned int y = 0; y < my; ++y) {
                if (!Serializer<U>::deserialize(result(x, y), d[x * my + y])) return false;
            }
        }
        return true;
    }

    static bool deserializeFrom(const Value& val, const std::string& key,
                                ctr::Vector2D<U>& result) {
        return priv::Serializer<ctr::Vector2D<U>>::deserializeFrom(val, key, result, &deserialize);
    }

    static bool deserializeStream(std::istream& stream, ctr::Vector2D<U>& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) return false;
        return deserialize(result, val);
    }

    static bool serializeStream(std::ostream& stream, const ctr::Vector2D<U>& value, unsigned int,
                                unsigned int) {
        const Value val = serialize(value);
        stream << val;
        return stream.good();
    }
};

template<typename... Ts>
struct Serializer<std::variant<Ts...>, false> {
private:
    struct HelperBase {
        virtual bool deserialize(const Value& input, std::variant<Ts...>& v) const = 0;
    };

    template<typename U>
    struct Helper : public HelperBase {
        virtual bool deserialize(const Value& input, std::variant<Ts...>& v) const override {
            v.template emplace<U>();
            return Serializer<U>::deserialize(std::get<U>(v), input);
        }
    };

public:
    static bool deserialize(std::variant<Ts...>& result, const Value& v) {
        const Group* group = v.getAsGroup();
        if (!group) return false;
        const Value* i = group->getField("index");
        const Value* d = group->getField("data");
        if (!i || !d) return false;
        if (i->getType() != Value::Type::Integer) return false;
        const unsigned int index = *i->getAsInteger();
        if (index >= sizeof...(Ts)) return false;

        static const std::tuple<Helper<Ts>...> helperTuple(Helper<Ts>{}...);
        static const HelperBase* helpers[] = {&std::get<Helper<Ts>>(helperTuple)...};
        return helpers[index]->deserialize(*d, result);
    }

    static bool deserializeFrom(const Value& val, const std::string& name,
                                std::variant<Ts...>& result) {
        return priv::Serializer<std::variant<Ts...>>::deserializeFrom(
            val, name, result, &deserialize);
    }

    static Value serialize(const std::variant<Ts...>& value) {
        Group result;
        result.addField("index", value.index());

        static const auto visitor = [](const auto& c) -> Value {
            return Serializer<std::decay_t<decltype(c)>>::serialize(c);
        };
        result.addField("data", std::visit(visitor, value));

        return {result};
    }

    static void serializeInto(Group& result, const std::string& name,
                              const std::variant<Ts...>& value) {
        priv::Serializer<std::variant<Ts...>>::serializeInto(result, name, value, &serialize);
    }

    static bool deserializeStream(std::istream& stream, std::variant<Ts...>& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) return false;
        return deserialize(result, val);
    }

    static bool serializeStream(std::ostream& stream, const std::variant<Ts...>& value,
                                unsigned int, unsigned int) {
        const Value val = serialize(value);
        stream << val;
        return stream.good();
    }
};

template<typename U, typename V>
struct Serializer<std::pair<U, V>, false> {
    static Value serialize(const std::pair<U, V>& v) {
        Group g;
        g.addField("first", Serializer<U>::serialize(v.first));
        g.addField("second", Serializer<V>::serialize(v.second));
        return {g};
    }

    static void serializeInto(const std::string& key, Group& g, const std::pair<U, V>& val) {
        priv::Serializer<std::pair<U, V>>::serializeInto(g, key, val, &serialize);
    }

    static bool deserialize(std::pair<U, V>& result, const Value& val) {
        const Group* rg = val.getAsGroup();
        if (rg == nullptr) return false;
        const Group& g = *rg;
        const Value* f = g.getField("first");
        const Value* s = g.getField("second");
        if (!f || !s) return false;
        if (!Serializer<U>::deserialize(result.first, *f)) return false;
        if (!Serializer<V>::deserialize(result.second, *s)) return false;
        return true;
    }

    static bool deserializeFrom(const Value& val, const std::string& key, std::pair<U, V>& result) {
        return priv::Serializer<std::pair<U, V>>::deserializeFrom(val, key, result, &deserialize);
    }

    static bool deserializeStream(std::istream& stream, std::pair<U, V>& result) {
        json::Loader loader(stream);
        Value val(0);
        if (!loader.loadValue(val)) return false;
        return deserialize(result, val);
    }

    static bool serializeStream(std::ostream& stream, const std::pair<U, V>& value,
                                unsigned int tabSize, unsigned int currentIndent) {
        stream << "{ ";
        stream << "\"first\": ";
        if (!Serializer<U>::serializeStream(stream, value.first, tabSize, currentIndent + tabSize))
            return false;
        stream << ", \"second\": ";
        if (!Serializer<V>::serializeStream(stream, value.second, tabSize, currentIndent + tabSize))
            return false;
        stream << " }";
        return stream.good();
    }
};

} // namespace json
} // namespace serial
} // namespace bl

#endif
