#ifndef BLIB_FILES_BINARY_SERIALIZER_HPP
#define BLIB_FILES_BINARY_SERIALIZER_HPP

#include <BLIB/Containers/Vector2d.hpp>
#include <BLIB/Scripts/Value.hpp>
#include <BLIB/Serialization/Binary/InputStream.hpp>
#include <BLIB/Serialization/Binary/OutputStream.hpp>
#include <BLIB/Serialization/SerializableObject.hpp>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <array>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>

namespace bl
{
namespace serial
{
namespace binary
{
/**
 * @brief Helper struct for serializing types into binary streams. Integral types, strings, vector,
 *        and SerializableObjects are supported out of the box. Specializations may be provided for
 *        user defined types
 *
 * @tparam T The type to serialize
 * @tparam bool Helper for integral specialization. Can be ignored
 * @ingroup Binary
 */
template<typename T, bool = std::is_integral<T>::value || std::is_enum<T>::value>
struct Serializer {
    /**
     * @brief Serialize the given value to the given file
     *
     * @param output The file to write to
     * @param value The value to write
     * @return True if written, false on error
     */
    static bool serialize(OutputStream& output, const T& value);

    /**
     * @brief Reads the value from input into result
     *
     * @param input The file to read from
     * @param result The object to read into
     * @return True if read, false on error
     */
    static bool deserialize(InputStream& input, T& result);

    /**
     * @brief Returns the size of the value when serialized
     *
     * @param value The value to be serialized
     * @return std::uint32_t The size of the value when serialized
     */
    static std::uint32_t size(const T& value);
};

///////////////////////////// INLINE FUNCTIONS ////////////////////////////////////

template<typename T>
struct Serializer<T, false> {
    static bool serialize(OutputStream& output, const T& value) {
        return SerializableObjectBase::get<T>().serializeBinary(output, &value);
    }

    static bool deserialize(InputStream& input, T& result) {
        return SerializableObjectBase::get<T>().deserializeBinary(input, &result);
    }

    static bool serializePacked(OutputStream& output, const T& value) {
        return SerializableObjectBase::get<T>().serializePackedBinary(output, &value);
    }

    static bool deserializePacked(InputStream& input, T& result) {
        return SerializableObjectBase::get<T>().deserializePackedBinary(input, &result);
    }

    static std::uint32_t size(const T& value) {
        return SerializableObjectBase::get<T>().binarySize(&value);
    }

private:
};

template<typename T>
struct Serializer<T, true> {
    static bool serialize(OutputStream& output, const T& value) {
        if constexpr (std::is_enum<T>::value) {
            return output.write<std::underlying_type_t<T>>(
                static_cast<std::underlying_type_t<T>>(value));
        }
        else if constexpr (std::is_same_v<T, bool>) { return output.write<std::uint8_t>(value); }
        else { return output.write<T>(value); }
    }

    static bool deserialize(InputStream& input, T& result) {
        if constexpr (std::is_enum<T>::value) {
            return input.read<std::underlying_type_t<T>>(
                *reinterpret_cast<std::underlying_type_t<T>*>(&result));
        }
        else if constexpr (std::is_same_v<T, bool>) {
            std::uint8_t v;
            if (!input.read<std::uint8_t>(v)) return false;
            result = v != 0;
            return true;
        }
        else { return input.read<T>(result); }
    }

    static std::uint32_t size(const T&) { return sizeof(T); }
};

template<>
struct Serializer<bool, false> {
    static bool serialize(OutputStream& output, const bool& value) {
        return output.write<std::uint8_t>(value ? 1 : 0);
    }

    static bool deserialize(InputStream& input, bool& value) {
        std::uint8_t v;
        if (!input.read<std::uint8_t>(v)) return false;
        value = v == 1;
        return true;
    }

    static std::uint32_t size(const bool&) { return sizeof(std::uint8_t); }
};

template<>
struct Serializer<std::string> {
    static bool serialize(OutputStream& output, const std::string& value) {
        return output.write(value);
    }

    static bool deserialize(InputStream& input, std::string& result) { return input.read(result); }

    static std::uint32_t size(const std::string& s) {
        return static_cast<std::uint32_t>(sizeof(std::uint32_t) + s.size());
    }
};

template<>
struct Serializer<float> {
    static bool serialize(OutputStream& output, const float& value) {
        return output.write(std::to_string(value));
    }

    static bool deserialize(InputStream& input, float& result) {
        std::string str;
        if (!input.read(str)) return false;
        try {
            result = std::stof(str);
            return true;
        } catch (...) { return false; }
    }

    static std::uint32_t size(const float& s) {
        return Serializer<std::string>::size(std::to_string(s));
    }
};

template<typename U, std::size_t N>
struct Serializer<U[N], false> {
    static bool serialize(OutputStream& out, const U* arr) {
        if (!out.write<std::uint32_t>(N)) return false;
        for (std::size_t i = 0; i < N; ++i) {
            if (!Serializer<U>::serialize(out, arr[i])) return false;
        }
        return true;
    }

    static bool deserialize(InputStream& in, U* arr) {
        std::uint32_t n = 0;
        if (!in.read<std::uint32_t>(n)) return false;
        if (n != N) return false;
        for (std::size_t i = 0; i < N; ++i) {
            if (!Serializer<U>::deserialize(in, arr[i])) return false;
        }
        return true;
    }

    static std::size_t size(const U* arr) {
        std::size_t s = sizeof(std::uint32_t);
        for (std::size_t i = 0; i < N; ++i) { s += Serializer<U>::size(arr[i]); }
        return s;
    }
};

template<typename U, std::size_t N>
struct Serializer<std::array<U, N>, false> {
    using T = std::array<U, N>;

    static bool serialize(OutputStream& out, const T& arr) {
        if (!out.write<std::uint32_t>(N)) return false;
        for (std::size_t i = 0; i < N; ++i) {
            if (!Serializer<U>::serialize(out, arr[i])) return false;
        }
        return true;
    }

    static bool deserialize(InputStream& in, T& arr) {
        std::uint32_t n = 0;
        if (!in.read<std::uint32_t>(n)) return false;
        if (n != N) return false;
        for (std::size_t i = 0; i < N; ++i) {
            if (!Serializer<U>::deserialize(in, arr[i])) return false;
        }
        return true;
    }

    static std::size_t size(const T& arr) {
        std::size_t s = sizeof(std::uint32_t);
        for (std::size_t i = 0; i < N; ++i) { s += Serializer<U>::size(arr[i]); }
        return s;
    }
};

template<typename U>
struct Serializer<std::vector<U>, false> {
    static bool serialize(OutputStream& output, const std::vector<U>& value) {
        if (!output.write<std::uint32_t>(value.size())) return false;
        for (const U& v : value) {
            if (!Serializer<U>::serialize(output, v)) return false;
        }
        return true;
    }

    static bool deserialize(InputStream& input, std::vector<U>& value) {
        value.clear();
        std::uint32_t size;
        if (!input.read<std::uint32_t>(size)) return false;
        value.resize(size);
        for (unsigned int i = 0; i < size; ++i) {
            if (!Serializer<U>::deserialize(input, value[i])) return false;
        }
        return true;
    }

    static std::uint32_t size(const std::vector<U>& v) {
        std::uint32_t vs = 0;
        for (const U& u : v) { vs += Serializer<U>::size(u); }
        return sizeof(std::uint32_t) + vs;
    }
};

template<typename U>
struct Serializer<ctr::Vector2D<U>, false> {
    static bool serialize(OutputStream& output, const ctr::Vector2D<U>& val) {
        if (!output.write<std::uint32_t>(val.getWidth())) return false;
        if (!output.write<std::uint32_t>(val.getHeight())) return false;
        for (unsigned int x = 0; x < val.getWidth(); ++x) {
            for (unsigned int y = 0; y < val.getHeight(); ++y) {
                if (!Serializer<U>::serialize(output, val(x, y))) return false;
            }
        }
        return true;
    }

    static bool deserialize(InputStream& input, ctr::Vector2D<U>& result) {
        std::uint32_t w, h;
        if (!input.read<std::uint32_t>(w)) return false;
        if (!input.read<std::uint32_t>(h)) return false;
        result.setSize(w, h);
        for (unsigned int x = 0; x < w; ++x) {
            for (unsigned int y = 0; y < h; ++y) {
                if (!Serializer<U>::deserialize(input, result(x, y))) return false;
            }
        }
        return true;
    }

    static std::uint32_t size(const ctr::Vector2D<U>& val) {
        std::uint32_t s = sizeof(std::uint32_t) * 2;
        for (unsigned int x = 0; x < val.getWidth(); ++x) {
            for (unsigned int y = 0; y < val.getHeight(); ++y) {
                s += Serializer<U>::size(val(x, y));
            }
        }
        return s;
    }
};

template<typename TKey, typename TValue>
struct Serializer<std::unordered_map<TKey, TValue>, false> {
    static bool serialize(OutputStream& output, const std::unordered_map<TKey, TValue>& value) {
        if (!output.write<std::uint32_t>(value.size())) return false;
        for (const auto& pair : value) {
            if (!Serializer<TKey>::serialize(output, pair.first)) return false;
            if (!Serializer<TValue>::serialize(output, pair.second)) return false;
        }
        return true;
    }

    static bool deserialize(InputStream& input, std::unordered_map<TKey, TValue>& value) {
        std::uint32_t size;
        if (!input.read<std::uint32_t>(size)) return false;
        for (std::uint32_t i = 0; i < size; ++i) {
            TKey key;
            if (!Serializer<TKey>::deserialize(input, key)) return false;
            auto it = value.try_emplace(key).first;
            if (!Serializer<TValue>::deserialize(input, it->second)) return false;
        }
        return true;
    }

    static std::uint32_t size(const std::unordered_map<TKey, TValue>& value) {
        std::uint32_t ms = 0;
        for (const auto& pair : value) {
            ms += Serializer<TKey>::size(pair.first);
            ms += Serializer<TValue>::size(pair.second);
        }
        return sizeof(std::uint32_t) + ms;
    }
};

template<typename U>
struct Serializer<std::unordered_set<U>, false> {
    static bool serialize(OutputStream& output, const std::unordered_set<U>& value) {
        if (!output.write<std::uint32_t>(value.size())) return false;
        for (const U& v : value) {
            if (!Serializer<U>::serialize(output, v)) return false;
        }
        return true;
    }

    static bool deserialize(InputStream& input, std::unordered_set<U>& value) {
        value.clear();
        std::uint32_t size;
        if (!input.read<std::uint32_t>(size)) return false;
        for (unsigned int i = 0; i < size; ++i) {
            U k;
            if (!Serializer<U>::deserialize(input, k)) return false;
            value.insert(std::move(k));
        }
        return true;
    }

    static std::uint32_t size(const std::unordered_set<U>& v) {
        std::uint32_t vs = 0;
        for (const U& u : v) { vs += Serializer<U>::size(u); }
        return sizeof(std::uint32_t) + vs;
    }
};

template<typename U>
struct Serializer<sf::Vector2<U>, false> {
    using S = Serializer<U>;
    static bool serialize(OutputStream& output, const sf::Vector2<U>& v) {
        if (!S::serialize(output, v.x)) return false;
        return S::serialize(output, v.y);
    }

    static bool deserialize(InputStream& input, sf::Vector2<U>& v) {
        if (!S::deserialize(input, v.x)) return false;
        return S::deserialize(input, v.y);
    }

    static std::uint32_t size(const sf::Vector2<U>& v) { return S::size(v.x) + S::size(v.y); }
};

template<typename U>
struct Serializer<sf::Vector3<U>, false> {
    using S = Serializer<U>;
    static bool serialize(OutputStream& output, const sf::Vector3<U>& v) {
        if (!S::serialize(output, v.x)) return false;
        if (!S::serialize(output, v.y)) return false;
        return S::serialize(output, v.z);
    }

    static bool deserialize(InputStream& input, sf::Vector3<U>& v) {
        if (!S::deserialize(input, v.x)) return false;
        if (!S::deserialize(input, v.y)) return false;
        return S::deserialize(input, v.z);
    }

    static std::uint32_t size(const sf::Vector3<U>& v) { return S::size(v.x) * 3; }
};

template<>
struct Serializer<sf::IntRect> {
    static bool serialize(OutputStream& output, const sf::IntRect& r) {
        if (!output.write<std::int32_t>(r.left)) return false;
        if (!output.write<std::int32_t>(r.top)) return false;
        if (!output.write<std::int32_t>(r.width)) return false;
        return output.write<std::int32_t>(r.height);
    }

    static bool deserialize(InputStream& input, sf::IntRect& r) {
        if (!input.read<std::int32_t>(r.left)) return false;
        if (!input.read<std::int32_t>(r.top)) return false;
        if (!input.read<std::int32_t>(r.width)) return false;
        return input.read<std::int32_t>(r.height);
    }

    static std::uint32_t size(const sf::IntRect&) { return sizeof(std::int32_t) * 4; }
};

template<>
struct Serializer<sf::Vector2u> {
    static bool serialize(OutputStream& output, const sf::Vector2u& v) {
        if (!output.write<std::uint32_t>(v.x)) return false;
        return output.write<std::uint32_t>(v.y);
    }

    static bool deserialize(InputStream& input, sf::Vector2u& v) {
        if (!input.read<std::uint32_t>(v.x)) return false;
        return input.read<std::uint32_t>(v.y);
    }

    static std::uint32_t size(const sf::Vector2u&) { return sizeof(std::uint32_t) * 2; }
};

template<typename U, typename V>
struct Serializer<std::pair<U, V>, false> {
    static bool serialize(OutputStream& output, const std::pair<U, V>& v) {
        if (!Serializer<U>::serialize(output, v.first)) return false;
        return Serializer<V>::serialize(output, v.second);
    }

    static bool deserialize(InputStream& input, std::pair<U, V>& v) {
        if (!Serializer<U>::deserialize(input, v.first)) return false;
        return Serializer<V>::deserialize(input, v.second);
    }

    static std::uint32_t size(const std::pair<U, V>& v) {
        return Serializer<U>::size(v.first) + Serializer<V>::size(v.second);
    }
};

template<typename... Ts>
struct Serializer<std::variant<Ts...>, false> {
private:
    struct HelperBase {
        virtual bool deserialize(InputStream& input, std::variant<Ts...>& v) const = 0;
    };

    template<typename U>
    struct Helper : public HelperBase {
        virtual bool deserialize(InputStream& input, std::variant<Ts...>& v) const override {
            v.template emplace<U>();
            return Serializer<U>::deserialize(input, std::get<U>(v));
        }
    };

public:
    static bool serialize(OutputStream& output, const std::variant<Ts...>& v) {
        const auto visitor = [&output](const auto& c) -> bool {
            return Serializer<std::decay_t<decltype(c)>>::serialize(output, c);
        };

        if (!output.write<std::uint16_t>(v.index())) return false;
        return std::visit(visitor, v);
    }

    static bool deserialize(InputStream& input, std::variant<Ts...>& v) {
        static const std::tuple<Helper<Ts>...> helperTuple(Helper<Ts>{}...);
        static const HelperBase* helpers[] = {&std::get<Helper<Ts>>(helperTuple)...};

        std::uint16_t i = 0;
        if (!input.read<std::uint16_t>(i)) return false;
        if (i >= sizeof...(Ts)) return false;
        return helpers[i]->deserialize(input, v);
    }

    static std::uint32_t size(const std::variant<Ts...>& v) {
        static constexpr auto visitor = [](const auto& c) -> std::uint32_t {
            return Serializer<std::decay_t<decltype(c)>>::size(c);
        };

        return std::visit(visitor, v) + sizeof(std::uint16_t);
    }
};

template<typename U>
struct Serializer<U*, false> {
    using T = U*;

    static bool serialize(OutputStream& output, const T v) {
        if (!v) return false;
        return Serializer<U>::serialize(output, *v);
    }

    static bool deserialize(InputStream& input, T v) {
        if (!v) return false;
        return Serializer<U>::deserialize(input, *v);
    }

    static std::uint32_t size(const T v) {
        if (!v) return 0;
        return Serializer<U>::size(*v);
    }
};

template<>
struct Serializer<script::Value, false> {
    static bool serialize(OutputStream& output, const script::Value& v) {
        const script::PrimitiveValue& value = v.value();
        if (!Serializer<script::PrimitiveValue::Type>::serialize(output, value.getType()))
            return false;
        if (!supported(v.value().getType())) return true;

        switch (v.value().getType()) {
        case script::PrimitiveValue::TBool:
            if (!Serializer<bool>::serialize(output, value.getAsBool())) return false;
            break;
        case script::PrimitiveValue::TInteger:
            if (!output.write<std::int32_t>(value.getAsInt())) return false;
            break;
        case script::PrimitiveValue::TFloat:
            if (!Serializer<float>::serialize(output, value.getAsFloat())) return false;
            break;
        case script::PrimitiveValue::TString:
            if (!Serializer<std::string>::serialize(output, value.getAsString())) return false;
            break;
        case script::PrimitiveValue::TArray:
            if (!output.write<std::uint32_t>(
                    static_cast<std::uint32_t>(value.getAsArray().size()))) {
                return false;
            }
            for (const script::Value& av : value.getAsArray()) {
                if (!serialize(output, av)) return false;
            }
            break;
        default:
            break;
        }

        if (!output.write<std::uint32_t>(v.allProperties().size())) return false;
        for (const auto& prop : v.allProperties()) {
            if (!Serializer<std::string>::serialize(output, prop.first)) return false;
            if (!Serializer<script::Value>::serialize(output, prop.second.deref())) return false;
        }

        return true;
    }

    static bool deserialize(InputStream& input, script::Value& v) {
        script::PrimitiveValue& value = v.value();
        script::PrimitiveValue::Type type;
        if (!Serializer<script::PrimitiveValue::Type>::deserialize(input, type)) return false;
        if (!supported(type)) {
            value = "<UNSUPPORTED TYPE>";
            return true;
        }

        switch (type) {
        case script::PrimitiveValue::TBool: {
            bool bv;
            if (!Serializer<bool>::deserialize(input, bv)) return false;
            value = bv;
            break;
        }
        case script::PrimitiveValue::TInteger: {
            std::int32_t i;
            if (!input.read<std::int32_t>(i)) return false;
            value = i;
            break;
        }
        case script::PrimitiveValue::TFloat: {
            float f;
            if (!Serializer<float>::deserialize(input, f)) return false;
            value = f;
            break;
        }
        case script::PrimitiveValue::TString: {
            std::string s;
            if (!Serializer<std::string>::deserialize(input, s)) return false;
            value = s;
            break;
        }
        case script::PrimitiveValue::TArray: {
            std::uint32_t s;
            if (!input.read<std::uint32_t>(s)) return false;
            value = script::ArrayValue{};
            value.getAsArray().resize(s);
            for (unsigned int i = 0; i < s; ++s) {
                if (!deserialize(input, value.getAsArray()[i])) return false;
            }
            break;
        }
        default:
            break;
        }

        std::uint32_t s;
        if (!input.read<std::uint32_t>(s)) return false;
        for (unsigned int i = 0; i < s; ++i) {
            std::string key;
            script::Value pv;
            if (!Serializer<std::string>::deserialize(input, key)) return false;
            if (!Serializer<script::Value>::deserialize(input, pv)) return false;
            v.setProperty(key, {std::move(pv)});
        }

        return true;
    }

    static std::uint32_t size(const script::Value& v) {
        std::uint32_t s = Serializer<script::PrimitiveValue::Type>::size(v.value().getType());
        if (!supported(v.value().getType())) return s;

        switch (v.value().getType()) {
        case script::PrimitiveValue::TBool:
            s += Serializer<bool>::size(true);
            break;
        case script::PrimitiveValue::TInteger:
            s += sizeof(std::int32_t);
            break;
        case script::PrimitiveValue::TFloat:
            s += Serializer<float>::size(v.value().getAsFloat());
            break;
        case script::PrimitiveValue::TString:
            s += Serializer<std::string>::size(v.value().getAsString());
            break;
        case script::PrimitiveValue::TArray:
            s += sizeof(std::uint32_t);
            for (const script::Value& av : v.value().getAsArray()) { s += size(av); }
            break;
        default:
            break;
        }

        s += sizeof(std::uint32_t);
        for (const auto& prop : v.allProperties()) {
            s += Serializer<std::string>::size(prop.first);
            s += Serializer<script::Value>::size(prop.second.deref());
        }

        return s;
    }

private:
    static bool supported(script::PrimitiveValue::Type type) {
        switch (type) {
        case script::PrimitiveValue::TBool:
        case script::PrimitiveValue::TInteger:
        case script::PrimitiveValue::TFloat:
        case script::PrimitiveValue::TString:
        case script::PrimitiveValue::TArray:
            return true;
        default:
            return false;
        }
    }
};

} // namespace binary
} // namespace serial
} // namespace bl

#endif
